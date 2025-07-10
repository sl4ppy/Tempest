#include "Renderer.h"
#include "OpenGLAPI.h"
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstring>
#include "Mesh.h"
#include "Graphics/glad/gl.h"

namespace Tempest {

Renderer::Renderer() 
    : m_initialized(false), m_recordingCommands(false), m_nextResourceID(1) {
    // Initialize render state
    m_renderState.projectionMatrix = glm::mat4(1.0f);
    m_renderState.viewMatrix = glm::mat4(1.0f);
    m_renderState.modelMatrix = glm::mat4(1.0f);
    m_renderState.clearColor = Color::Black();
    m_renderState.depthTestEnabled = true;
    m_renderState.blendMode = BlendMode::Alpha;
    m_renderState.viewportX = 0;
    m_renderState.viewportY = 0;
    m_renderState.viewportWidth = 1920;
    m_renderState.viewportHeight = 1080;
    
    // Initialize immediate state
    m_immediateState.vao = 0;
    m_immediateState.vbo = 0;
    m_immediateState.ebo = 0;
    m_immediateState.shader = nullptr;
    m_immediateState.active = false;
}

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize(const GraphicsConfig& config) {
    m_graphicsAPI = CreateGraphicsAPI(config.api);
    
    if (!m_graphicsAPI->Initialize(config)) {
        spdlog::error("Failed to initialize graphics API");
        return false;
    }
    
    // Initialize ShaderManager
    if (!m_shaderManager.Initialize(m_graphicsAPI.get())) {
        spdlog::error("Failed to initialize ShaderManager");
        return false;
    }
    
    // Create built-in shaders
    m_shaderManager.CreateBuiltInShaders();
    
    m_initialized = true;
    spdlog::info("Renderer initialized successfully with {} built-in shaders", m_shaderManager.GetShaderCount());
    return true;
}

void Renderer::Shutdown() {
    if (m_initialized) {
        CleanupImmediateMode();
        
        // Shutdown ShaderManager
        m_shaderManager.Shutdown();
        
        // Clean up resources
        m_shaders.clear();
        m_textures.clear();
        m_meshes.clear();
        
        if (m_graphicsAPI) {
            m_graphicsAPI->Shutdown();
        }
        
        m_initialized = false;
        spdlog::info("Renderer shutdown complete");
    }
}

void Renderer::CreateWindow(const WindowConfig& config) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return;
    }
    
    m_graphicsAPI->CreateWindow(config);
    
    // Update viewport
    m_renderState.viewportWidth = config.width;
    m_renderState.viewportHeight = config.height;
    SetViewport(0, 0, config.width, config.height);
}

void Renderer::DestroyWindow() {
    if (m_graphicsAPI) {
        m_graphicsAPI->DestroyWindow();
    }
}

bool Renderer::ShouldClose() const {
    return m_graphicsAPI ? m_graphicsAPI->ShouldClose() : true;
}

void Renderer::SwapBuffers() {
    if (!m_initialized) return;
    
    // Check if we have access to the SDL window through the GameEngine
    // For now, we'll delegate to the graphics API but this needs to be fixed
    // The proper solution is to have the GameEngine handle buffer swapping
    // since it has access to the SDL window
    if (m_graphicsAPI) {
        m_graphicsAPI->SwapBuffers();
    }
}

void Renderer::PollEvents() {
    if (m_graphicsAPI) {
        m_graphicsAPI->PollEvents();
    }
}

void Renderer::BeginFrame() {
    if (!m_initialized) return;
    
    // Only use immediate mode OpenGL calls if we're using OpenGL API
    if (m_graphicsAPI && dynamic_cast<OpenGLAPI*>(m_graphicsAPI.get()) != nullptr) {
        // Set clear color to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Clear command queue
    m_commandQueue.clear();
    m_recordingCommands = true;
    
    // Set default state through graphics API abstraction
    m_graphicsAPI->SetViewport(m_renderState.viewportX, m_renderState.viewportY, 
                               m_renderState.viewportWidth, m_renderState.viewportHeight);
    m_graphicsAPI->SetClearColor(m_renderState.clearColor);
    m_graphicsAPI->EnableDepthTest(m_renderState.depthTestEnabled);
    m_graphicsAPI->SetBlendMode(m_renderState.blendMode);
}

void Renderer::EndFrame() {
    if (!m_initialized) return;
    
    m_recordingCommands = false;
    
    // Execute all queued commands
    FlushCommandQueue();
    
    // Swap buffers
    SwapBuffers();
}

void Renderer::Clear(const Color& color) {
    if (m_recordingCommands) {
        RenderCommand cmd;
        cmd.type = RenderCommand::Type::Clear;
        cmd.SetData(color);
        m_commandQueue.push_back(cmd);
    } else if (m_graphicsAPI) {
        m_graphicsAPI->Clear(color);
    }
}

void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    m_renderState.viewportX = x;
    m_renderState.viewportY = y;
    m_renderState.viewportWidth = width;
    m_renderState.viewportHeight = height;
    
    if (m_recordingCommands) {
        RenderCommand cmd;
        cmd.type = RenderCommand::Type::SetViewport;
        struct ViewportData { uint32_t x, y, width, height; } data{x, y, width, height};
        cmd.SetData(data);
        m_commandQueue.push_back(cmd);
    } else if (m_graphicsAPI) {
        m_graphicsAPI->SetViewport(x, y, width, height);
    }
}

void Renderer::SetClearColor(const Color& color) {
    m_renderState.clearColor = color;
    if (m_graphicsAPI) {
        m_graphicsAPI->SetClearColor(color);
    }
}

void Renderer::SetProjectionMatrix(const glm::mat4& projection) {
    m_renderState.projectionMatrix = projection;
}

void Renderer::SetViewMatrix(const glm::mat4& view) {
    m_renderState.viewMatrix = view;
}

void Renderer::SetModelMatrix(const glm::mat4& model) {
    m_renderState.modelMatrix = model;
}

void Renderer::SetBlendMode(BlendMode mode) {
    m_renderState.blendMode = mode;
    
    if (m_recordingCommands) {
        RenderCommand cmd;
        cmd.type = RenderCommand::Type::SetBlendMode;
        cmd.SetData(mode);
        m_commandQueue.push_back(cmd);
    } else if (m_graphicsAPI) {
        m_graphicsAPI->SetBlendMode(mode);
    }
}

void Renderer::EnableDepthTest(bool enable) {
    m_renderState.depthTestEnabled = enable;
    
    if (m_recordingCommands) {
        RenderCommand cmd;
        cmd.type = RenderCommand::Type::EnableDepthTest;
        cmd.SetData(enable);
        m_commandQueue.push_back(cmd);
    } else if (m_graphicsAPI) {
        m_graphicsAPI->EnableDepthTest(enable);
    }
}

void Renderer::DrawMesh(const Mesh& mesh, const Shader& shader) {
    // TODO: Implement mesh drawing
    spdlog::warn("DrawMesh not yet implemented");
}

void Renderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const Color& color, float width) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return;
    }
    
    // Only use immediate mode OpenGL calls if we're using OpenGL API
    if (m_graphicsAPI && dynamic_cast<OpenGLAPI*>(m_graphicsAPI.get()) != nullptr) {
        // Save current matrices
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(m_renderState.projectionMatrix));
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(m_renderState.viewMatrix * m_renderState.modelMatrix));
        
        // Set line width
        glLineWidth(width);
        
        // Set color
        glColor4f(color.r, color.g, color.b, color.a);
        
        // Draw line using immediate mode
        glBegin(GL_LINES);
        glVertex3f(start.x, start.y, start.z);
        glVertex3f(end.x, end.y, end.z);
        glEnd();
        
        // Restore matrices
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    } else {
        // For mock API, just log the call
        spdlog::debug("DrawLine called in mock mode: ({}, {}, {}) -> ({}, {}, {})", 
                     start.x, start.y, start.z, end.x, end.y, end.z);
    }
    
    // Reset color to white
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void Renderer::DrawPoint(const glm::vec3& position, const Color& color, float size) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return;
    }
    
    // Only use immediate mode OpenGL calls if we're using OpenGL API
    if (m_graphicsAPI && dynamic_cast<OpenGLAPI*>(m_graphicsAPI.get()) != nullptr) {
        // Save current matrices
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(m_renderState.projectionMatrix));
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(m_renderState.viewMatrix * m_renderState.modelMatrix));
        
        // Set point size
        glPointSize(size);
        
        // Set color
        glColor4f(color.r, color.g, color.b, color.a);
        
        // Draw point using immediate mode
        glBegin(GL_POINTS);
        glVertex3f(position.x, position.y, position.z);
        glEnd();
        
        // Restore matrices
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    } else {
        // For mock API, just log the call
        spdlog::debug("DrawPoint called in mock mode: ({}, {}, {})", 
                     position.x, position.y, position.z);
    }
    
    // Reset color to white
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void Renderer::DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const Color& color) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return;
    }
    
    // Only use immediate mode OpenGL calls if we're using OpenGL API
    if (m_graphicsAPI && dynamic_cast<OpenGLAPI*>(m_graphicsAPI.get()) != nullptr) {
        // Save current matrices
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(m_renderState.projectionMatrix));
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(m_renderState.viewMatrix * m_renderState.modelMatrix));
        
        // Set color
        glColor4f(color.r, color.g, color.b, color.a);
        
        // Draw triangle using immediate mode
        glBegin(GL_TRIANGLES);
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
        glEnd();
        
        // Restore matrices
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        
        // Reset color to white
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        // For mock API, just log the call
        spdlog::debug("DrawTriangle called in mock mode: ({}, {}, {}) -> ({}, {}, {}) -> ({}, {}, {})", 
                     a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z);
    }
}

void Renderer::DrawRectangle(const glm::vec2& position, const glm::vec2& size, const Color& color) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return;
    }
    
    // Draw rectangle as two triangles
    glm::vec3 topLeft(position.x, position.y, 0.0f);
    glm::vec3 topRight(position.x + size.x, position.y, 0.0f);
    glm::vec3 bottomLeft(position.x, position.y + size.y, 0.0f);
    glm::vec3 bottomRight(position.x + size.x, position.y + size.y, 0.0f);
    
    // First triangle: top-left, top-right, bottom-left
    DrawTriangle(topLeft, topRight, bottomLeft, color);
    
    // Second triangle: top-right, bottom-right, bottom-left
    DrawTriangle(topRight, bottomRight, bottomLeft, color);
}

void Renderer::DrawCircle(const glm::vec2& center, float radius, const Color& color, uint32_t segments) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return;
    }
    
    if (segments < 3) {
        segments = 3;
    }
    
    // Draw circle as triangles from center point
    glm::vec3 centerPoint(center.x, center.y, 0.0f);
    
    for (uint32_t i = 0; i < segments; ++i) {
        float angle1 = 2.0f * 3.14159f * i / segments;
        float angle2 = 2.0f * 3.14159f * (i + 1) / segments;
        
        glm::vec3 point1(center.x + radius * cos(angle1), center.y + radius * sin(angle1), 0.0f);
        glm::vec3 point2(center.x + radius * cos(angle2), center.y + radius * sin(angle2), 0.0f);
        
        DrawTriangle(centerPoint, point1, point2, color);
    }
}

void Renderer::BeginImmediate() {
    if (m_immediateState.active) {
        spdlog::warn("Immediate mode already active");
        return;
    }
    
    SetupImmediateMode();
    m_immediateState.active = true;
    m_immediateState.vertices.clear();
    m_immediateState.indices.clear();
}

void Renderer::EndImmediate() {
    if (!m_immediateState.active) {
        spdlog::warn("Immediate mode not active");
        return;
    }
    
    // Draw accumulated vertices
    if (!m_immediateState.vertices.empty() && !m_immediateState.indices.empty()) {
        // Update buffers with vertex data
        if (m_graphicsAPI) {
            m_graphicsAPI->BindVertexArray(m_immediateState.vao);
            m_graphicsAPI->UpdateVertexBuffer(m_immediateState.vbo, 
                                            m_immediateState.vertices.data(), 
                                            m_immediateState.vertices.size() * sizeof(float));
            m_graphicsAPI->BindIndexBuffer(m_immediateState.ebo);
            m_graphicsAPI->UpdateIndexBuffer(m_immediateState.ebo,
                                           m_immediateState.indices.data(),
                                           m_immediateState.indices.size() * sizeof(uint32_t));
            
            // Use immediate mode shader if available
            if (m_immediateState.shader) {
                m_immediateState.shader->Use();
                
                // Set matrices
                m_immediateState.shader->SetProjectionMatrix(m_renderState.projectionMatrix);
                m_immediateState.shader->SetViewMatrix(m_renderState.viewMatrix);
                m_immediateState.shader->SetModelMatrix(m_renderState.modelMatrix);
                
                // Draw the accumulated triangles
                m_graphicsAPI->DrawElements(0x0004, // GL_TRIANGLES
                                          m_immediateState.indices.size(), 
                                          0x1405, // GL_UNSIGNED_INT
                                          nullptr);
            }
        }
    }
    
    m_immediateState.active = false;
}

void Renderer::ImmediateVertex(const glm::vec3& position, const Color& color) {
    if (!m_immediateState.active) {
        spdlog::warn("Immediate mode not active");
        return;
    }
    
    // Add vertex data (position + color)
    m_immediateState.vertices.push_back(position.x);
    m_immediateState.vertices.push_back(position.y);
    m_immediateState.vertices.push_back(position.z);
    m_immediateState.vertices.push_back(color.r);
    m_immediateState.vertices.push_back(color.g);
    m_immediateState.vertices.push_back(color.b);
    m_immediateState.vertices.push_back(color.a);
}

void Renderer::ImmediateTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const Color& color) {
    if (!m_immediateState.active) {
        spdlog::warn("Immediate mode not active");
        return;
    }
    
    uint32_t baseIndex = m_immediateState.vertices.size() / 7; // 7 floats per vertex
    
    ImmediateVertex(a, color);
    ImmediateVertex(b, color);
    ImmediateVertex(c, color);
    
    // Add indices for triangle
    m_immediateState.indices.push_back(baseIndex);
    m_immediateState.indices.push_back(baseIndex + 1);
    m_immediateState.indices.push_back(baseIndex + 2);
}

uint32_t Renderer::CreateShader(const std::string& vertexSource, const std::string& fragmentSource) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return 0;
    }
    
    if (!m_graphicsAPI) {
        spdlog::error("Graphics API not available");
        return 0;
    }
    
    try {
        // Create new shader object
        auto shader = std::make_unique<Shader>();
        
        // Create shader using the Shader class
        if (!shader->Create(vertexSource, fragmentSource, m_graphicsAPI.get())) {
            spdlog::error("Failed to create shader");
            return 0;
        }
        
        // Generate resource ID and store shader
        uint32_t resourceId = GenerateResourceID();
        m_shaders[resourceId] = std::move(shader);
        
        spdlog::debug("Created shader with ID: {}", resourceId);
        return resourceId;
        
    } catch (const std::exception& e) {
        spdlog::error("Exception creating shader: {}", e.what());
        return 0;
    }
}

void Renderer::DeleteShader(uint32_t shader) {
    if (m_shaders.find(shader) != m_shaders.end()) {
        m_shaders.erase(shader);
    }
}

void Renderer::UseShader(uint32_t shader) {
    if (!m_initialized || !m_graphicsAPI) {
        spdlog::error("Renderer not initialized or Graphics API not available");
        return;
    }
    
    auto it = m_shaders.find(shader);
    if (it != m_shaders.end()) {
        uint32_t program = it->second->GetProgram();
        m_graphicsAPI->UseProgram(program);
        spdlog::debug("Using shader program: {}", program);
    } else {
        spdlog::warn("Shader with ID {} not found", shader);
    }
}

// Enhanced shader management methods using ShaderManager
std::shared_ptr<Shader> Renderer::CreateShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return nullptr;
    }
    
    return m_shaderManager.CreateShader(name, vertexSource, fragmentSource);
}

std::shared_ptr<Shader> Renderer::CreateShaderFromFiles(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return nullptr;
    }
    
    return m_shaderManager.CreateShaderFromFiles(name, vertexPath, fragmentPath);
}

std::shared_ptr<Shader> Renderer::CreateBuiltInShader(const std::string& name, const std::string& shaderType) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return nullptr;
    }
    
    return m_shaderManager.CreateBuiltInShader(name, shaderType);
}

std::shared_ptr<Shader> Renderer::GetShader(const std::string& name) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return nullptr;
    }
    
    return m_shaderManager.GetShader(name);
}

bool Renderer::HasShader(const std::string& name) const {
    if (!m_initialized) {
        return false;
    }
    
    return m_shaderManager.HasShader(name);
}

void Renderer::RemoveShader(const std::string& name) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return;
    }
    
    m_shaderManager.RemoveShader(name);
}

void Renderer::UseShader(const std::string& name) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return;
    }
    
    auto shader = m_shaderManager.GetShader(name);
    if (shader) {
        UseShader(shader);
    } else {
        spdlog::warn("Shader '{}' not found", name);
    }
}

void Renderer::UseShader(std::shared_ptr<Shader> shader) {
    if (!m_initialized || !shader) {
        spdlog::error("Renderer not initialized or invalid shader");
        return;
    }
    
    m_currentShader = shader;
    shader->Use();
    
    // Set standard uniforms
    shader->SetProjectionMatrix(m_renderState.projectionMatrix);
    shader->SetViewMatrix(m_renderState.viewMatrix);
    shader->SetModelMatrix(m_renderState.modelMatrix);
    
    spdlog::debug("Using shader program: {}", shader->GetProgram());
}

uint32_t Renderer::CreateTexture(uint32_t width, uint32_t height, const void* data) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return 0;
    }
    
    if (!m_graphicsAPI) {
        spdlog::error("Graphics API not available");
        return 0;
    }
    
    try {
        // Create OpenGL texture
        uint32_t textureID = m_graphicsAPI->CreateTexture();
        if (textureID == 0) {
            spdlog::error("Failed to create texture");
            return 0;
        }
        
        // Bind texture and set data
        m_graphicsAPI->BindTexture(0x0DE1, textureID); // GL_TEXTURE_2D
        
        // Set texture data
        m_graphicsAPI->SetTextureData(0x0DE1, // GL_TEXTURE_2D
                                     0,       // level
                                     0x1908,  // GL_RGBA (internal format)
                                     width, 
                                     height,
                                     0x1908,  // GL_RGBA (format)
                                     0x1401,  // GL_UNSIGNED_BYTE (type)
                                     data);
        
        // Set texture parameters
        m_graphicsAPI->SetTextureParameter(0x0DE1, 0x2801, 0x2601); // GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR
        m_graphicsAPI->SetTextureParameter(0x0DE1, 0x2800, 0x2601); // GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR
        m_graphicsAPI->SetTextureParameter(0x0DE1, 0x2802, 0x2901); // GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT
        m_graphicsAPI->SetTextureParameter(0x0DE1, 0x2803, 0x2901); // GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT
        
        // Generate resource ID and store texture
        uint32_t resourceId = GenerateResourceID();
        auto texture = std::make_unique<Texture>();
        texture->SetTextureID(textureID);
        texture->SetDimensions(width, height);
        m_textures[resourceId] = std::move(texture);
        
        spdlog::debug("Created texture with ID: {} ({}x{})", resourceId, width, height);
        return resourceId;
        
    } catch (const std::exception& e) {
        spdlog::error("Exception creating texture: {}", e.what());
        return 0;
    }
}

void Renderer::DeleteTexture(uint32_t texture) {
    if (m_textures.find(texture) != m_textures.end()) {
        m_textures.erase(texture);
    }
}

void Renderer::BindTexture(uint32_t texture, uint32_t unit) {
    if (!m_initialized || !m_graphicsAPI) {
        spdlog::error("Renderer not initialized or Graphics API not available");
        return;
    }
    
    auto it = m_textures.find(texture);
    if (it != m_textures.end()) {
        uint32_t textureID = it->second->GetTextureID();
        
        // Activate texture unit first (if supported)
        // For simplicity, we'll just bind to GL_TEXTURE_2D
        m_graphicsAPI->BindTexture(0x0DE1, textureID); // GL_TEXTURE_2D
        
        spdlog::debug("Bound texture ID: {} to unit: {}", textureID, unit);
    } else {
        spdlog::warn("Texture with ID {} not found", texture);
    }
}

uint32_t Renderer::CreateMesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices) {
    if (!m_initialized) {
        spdlog::error("Renderer not initialized");
        return 0;
    }
    
    if (!m_graphicsAPI) {
        spdlog::error("Graphics API not available");
        return 0;
    }
    
    if (vertices.empty()) {
        spdlog::error("Cannot create mesh with empty vertices");
        return 0;
    }
    
    try {
        // Create vertex array object
        uint32_t vao = m_graphicsAPI->CreateVertexArray();
        if (vao == 0) {
            spdlog::error("Failed to create vertex array");
            return 0;
        }
        
        // Create vertex buffer
        uint32_t vbo = m_graphicsAPI->CreateVertexBuffer(vertices.data(), vertices.size() * sizeof(float));
        if (vbo == 0) {
            spdlog::error("Failed to create vertex buffer");
            m_graphicsAPI->DeleteVertexArray(vao);
            return 0;
        }
        
        // Create index buffer if indices provided
        uint32_t ebo = 0;
        if (!indices.empty()) {
            ebo = m_graphicsAPI->CreateIndexBuffer(indices.data(), indices.size() * sizeof(uint32_t));
            if (ebo == 0) {
                spdlog::error("Failed to create index buffer");
                m_graphicsAPI->DeleteVertexArray(vao);
                m_graphicsAPI->DeleteBuffer(vbo);
                return 0;
            }
        }
        
        // Bind VAO and set up vertex attributes
        m_graphicsAPI->BindVertexArray(vao);
        m_graphicsAPI->BindVertexBuffer(vbo);
        if (ebo != 0) {
            m_graphicsAPI->BindIndexBuffer(ebo);
        }
        
        // Set vertex attributes (assuming position + color: 3 floats pos + 4 floats color = 7 floats total)
        m_graphicsAPI->SetVertexAttribute(0, 3, 0x1406, false, 7 * sizeof(float), nullptr); // position
        m_graphicsAPI->SetVertexAttribute(1, 4, 0x1406, false, 7 * sizeof(float), 
                                        reinterpret_cast<const void*>(3 * sizeof(float))); // color
        
        // Unbind VAO
        m_graphicsAPI->BindVertexArray(0);
        
        // Generate resource ID and store mesh
        uint32_t resourceId = GenerateResourceID();
        auto mesh = std::make_unique<Mesh>();
        mesh->SetVAO(vao);
        mesh->SetVBO(vbo);
        mesh->SetEBO(ebo);
        mesh->SetVertexCount(vertices.size() / 7); // 7 floats per vertex
        mesh->SetIndexCount(indices.size());
        m_meshes[resourceId] = std::move(mesh);
        
        spdlog::debug("Created mesh with ID: {} ({} vertices, {} indices)", 
                     resourceId, vertices.size() / 7, indices.size());
        return resourceId;
        
    } catch (const std::exception& e) {
        spdlog::error("Exception creating mesh: {}", e.what());
        return 0;
    }
}

void Renderer::DeleteMesh(uint32_t mesh) {
    if (m_meshes.find(mesh) != m_meshes.end()) {
        m_meshes.erase(mesh);
    }
}

void Renderer::DrawMesh(uint32_t mesh, uint32_t shader) {
    if (!m_initialized || !m_graphicsAPI) {
        spdlog::error("Renderer not initialized or Graphics API not available");
        return;
    }
    
    // Find mesh
    auto meshIt = m_meshes.find(mesh);
    if (meshIt == m_meshes.end()) {
        spdlog::warn("Mesh with ID {} not found", mesh);
        return;
    }
    
    // Find shader
    auto shaderIt = m_shaders.find(shader);
    if (shaderIt == m_shaders.end()) {
        spdlog::warn("Shader with ID {} not found", shader);
        return;
    }
    
    // Use shader program
    uint32_t program = shaderIt->second->GetProgram();
    m_graphicsAPI->UseProgram(program);
    
    // Set shader uniforms
    m_graphicsAPI->SetUniform(program, "projection", m_renderState.projectionMatrix);
    m_graphicsAPI->SetUniform(program, "view", m_renderState.viewMatrix);
    m_graphicsAPI->SetUniform(program, "model", m_renderState.modelMatrix);
    
    // Bind mesh VAO
    uint32_t vao = meshIt->second->GetVAO();
    m_graphicsAPI->BindVertexArray(vao);
    
    // Draw mesh
    if (meshIt->second->GetIndexCount() > 0) {
        // Draw with indices
        m_graphicsAPI->DrawElements(0x0004, // GL_TRIANGLES
                                  meshIt->second->GetIndexCount(),
                                  0x1405, // GL_UNSIGNED_INT
                                  nullptr);
    } else {
        // Draw without indices
        m_graphicsAPI->DrawArrays(0x0004, // GL_TRIANGLES
                                0,
                                meshIt->second->GetVertexCount());
    }
    
    // Unbind VAO
    m_graphicsAPI->BindVertexArray(0);
    
    spdlog::debug("Drew mesh ID: {} with shader ID: {}", mesh, shader);
}

bool Renderer::CheckError() {
    return m_graphicsAPI ? m_graphicsAPI->CheckError() : false;
}

std::string Renderer::GetErrorString() {
    return m_graphicsAPI ? m_graphicsAPI->GetErrorString() : "";
}

// Private helper methods
void Renderer::ExecuteCommand(const RenderCommand& command) {
    switch (command.type) {
        case RenderCommand::Type::Clear: {
            Color color = command.GetData<Color>();
            m_graphicsAPI->Clear(color);
            break;
        }
        case RenderCommand::Type::SetViewport: {
            struct ViewportData { uint32_t x, y, width, height; };
            auto data = command.GetData<ViewportData>();
            m_graphicsAPI->SetViewport(data.x, data.y, data.width, data.height);
            break;
        }
        case RenderCommand::Type::SetBlendMode: {
            BlendMode mode = command.GetData<BlendMode>();
            m_graphicsAPI->SetBlendMode(mode);
            break;
        }
        case RenderCommand::Type::EnableDepthTest: {
            bool enable = command.GetData<bool>();
            m_graphicsAPI->EnableDepthTest(enable);
            break;
        }
        case RenderCommand::Type::DrawMesh: {
            struct MeshDrawData { uint32_t meshId; uint32_t shaderId; };
            auto data = command.GetData<MeshDrawData>();
            DrawMesh(data.meshId, data.shaderId);
            break;
        }
        case RenderCommand::Type::DrawLine: {
            struct LineDrawData { 
                glm::vec3 start; 
                glm::vec3 end; 
                Color color; 
                float width; 
            };
            auto data = command.GetData<LineDrawData>();
            DrawLine(data.start, data.end, data.color, data.width);
            break;
        }
        case RenderCommand::Type::DrawPoint: {
            struct PointDrawData { 
                glm::vec3 position; 
                Color color; 
                float size; 
            };
            auto data = command.GetData<PointDrawData>();
            DrawPoint(data.position, data.color, data.size);
            break;
        }
        case RenderCommand::Type::SetShader: {
            uint32_t shaderId = command.GetData<uint32_t>();
            UseShader(shaderId);
            break;
        }
        case RenderCommand::Type::SetTexture: {
            struct TextureData { uint32_t textureId; uint32_t unit; };
            auto data = command.GetData<TextureData>();
            BindTexture(data.textureId, data.unit);
            break;
        }
        case RenderCommand::Type::SetUniform: {
            // Note: Uniform setting is complex and would need more sophisticated command data
            // For now, we'll just warn about it
            spdlog::warn("SetUniform command not yet implemented in command queue");
            break;
        }
        default:
            spdlog::warn("Unhandled render command type: {}", static_cast<int>(command.type));
            break;
    }
}

void Renderer::FlushCommandQueue() {
    for (const auto& command : m_commandQueue) {
        ExecuteCommand(command);
    }
    m_commandQueue.clear();
}

void Renderer::SetupImmediateMode() {
    if (m_immediateState.vao == 0) {
        m_immediateState.vao = m_graphicsAPI->CreateVertexArray();
        m_immediateState.vbo = m_graphicsAPI->CreateVertexBuffer(nullptr, 0);
        m_immediateState.ebo = m_graphicsAPI->CreateIndexBuffer(nullptr, 0);
        
        m_graphicsAPI->BindVertexArray(m_immediateState.vao);
        m_graphicsAPI->BindVertexBuffer(m_immediateState.vbo);
        m_graphicsAPI->BindIndexBuffer(m_immediateState.ebo);
        
        // Set vertex attributes (position + color)
        m_graphicsAPI->SetVertexAttribute(0, 3, 0x1406, false, 7 * sizeof(float), nullptr); // GL_FLOAT = 0x1406
        m_graphicsAPI->SetVertexAttribute(1, 4, 0x1406, false, 7 * sizeof(float), 
                                        reinterpret_cast<const void*>(3 * sizeof(float)));
        
        // Set up built-in shader for immediate mode
        m_immediateState.shader = m_shaderManager.GetShader("basic");
        if (!m_immediateState.shader) {
            spdlog::info("Creating built-in basic shader for immediate mode");
            m_immediateState.shader = m_shaderManager.CreateBuiltInShader("basic", "basic");
        }
    }
}

void Renderer::CleanupImmediateMode() {
    if (m_immediateState.vao != 0) {
        m_graphicsAPI->DeleteVertexArray(m_immediateState.vao);
        m_graphicsAPI->DeleteBuffer(m_immediateState.vbo);
        m_graphicsAPI->DeleteBuffer(m_immediateState.ebo);
        m_immediateState.vao = 0;
        m_immediateState.vbo = 0;
        m_immediateState.ebo = 0;
    }
}

uint32_t Renderer::GenerateResourceID() {
    return m_nextResourceID++;
}

} // namespace Tempest 