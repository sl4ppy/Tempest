#include "TubeRenderer.h"
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>

namespace Tempest {

TubeRenderer::TubeRenderer()
    : m_renderer(nullptr)
    , m_initialized(false)
    , m_cameraPosition(0.0f, -15.0f, 0.0f)
    , m_cameraTarget(0.0f, 0.0f, 0.0f)
    , m_tubeRadius(1.0f)
    , m_tubeDepth(10.0f)
    , m_segmentCount(16)
    , m_depthLevels(10)
    , m_wireframeEnabled(false)
    , m_depthTestEnabled(true)
    , m_blendingEnabled(true)
    , m_wireframeColor(0.5f, 0.5f, 0.5f, 0.3f) {
    
    // Initialize matrices
    m_projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
    m_viewMatrix = glm::lookAt(m_cameraPosition, m_cameraTarget, glm::vec3(0.0f, 0.0f, 1.0f));
    m_modelMatrix = glm::mat4(1.0f);
    
    // Initialize segment colors
    m_segmentColors.resize(16);
    for (uint8_t i = 0; i < 16; ++i) {
        float factor = static_cast<float>(i) / 16.0f;
        m_segmentColors[i] = glm::vec4(
            0.3f + 0.4f * factor,
            0.2f + 0.3f * factor,
            0.5f + 0.2f * (1.0f - factor),
            0.8f
        );
    }
}

TubeRenderer::~TubeRenderer() {
    Shutdown();
}

bool TubeRenderer::Initialize(Renderer* renderer) {
    if (!renderer) {
        spdlog::error("TubeRenderer: Invalid renderer pointer");
        return false;
    }
    
    m_renderer = renderer;
    
    try {
        CreateShaders();
        CreateMeshes();
        UpdateMatrices();
        
        m_initialized = true;
        spdlog::info("TubeRenderer initialized successfully");
        return true;
    }
    catch (const std::exception& e) {
        spdlog::error("TubeRenderer initialization failed: {}", e.what());
        return false;
    }
}

void TubeRenderer::Shutdown() {
    if (!m_initialized) return;
    
    m_tubeShader.reset();
    m_wireframeShader.reset();
    m_segmentShader.reset();
    m_tubeMesh.reset();
    m_wireframeMesh.reset();
    m_segmentMesh.reset();
    
    m_initialized = false;
    spdlog::info("TubeRenderer shut down");
}

void TubeRenderer::RenderTube(const TubeGeometry& geometry) {
    if (!m_initialized || !m_renderer) {
        spdlog::warn("TubeRenderer not initialized, skipping tube render");
        return;
    }
    
    // Set renderer state
    m_renderer->SetProjectionMatrix(m_projectionMatrix);
    m_renderer->SetViewMatrix(m_viewMatrix);
    m_renderer->SetModelMatrix(m_modelMatrix);
    m_renderer->EnableDepthTest(m_depthTestEnabled);
    m_renderer->SetBlendMode(m_blendingEnabled ? BlendMode::Alpha : BlendMode::None);
    
    // Render tube geometry
    RenderTubeGeometry(geometry);
    
    // Render wireframe if enabled
    if (m_wireframeEnabled) {
        RenderWireframeGeometry(geometry);
    }
}

void TubeRenderer::RenderTubeWireframe(const TubeGeometry& geometry) {
    if (!m_initialized || !m_renderer) return;
    
    // Set renderer state for wireframe
    m_renderer->SetProjectionMatrix(m_projectionMatrix);
    m_renderer->SetViewMatrix(m_viewMatrix);
    m_renderer->SetModelMatrix(m_modelMatrix);
    m_renderer->EnableDepthTest(m_depthTestEnabled);
    m_renderer->SetBlendMode(BlendMode::Alpha);
    
    // Render wireframe geometry
    RenderWireframeGeometry(geometry);
}

void TubeRenderer::RenderSegment(uint8_t segment, float depth, const glm::vec4& color) {
    if (!m_initialized || !m_renderer) return;
    
    // Set renderer state
    m_renderer->SetProjectionMatrix(m_projectionMatrix);
    m_renderer->SetViewMatrix(m_viewMatrix);
    m_renderer->SetModelMatrix(m_modelMatrix);
    m_renderer->EnableDepthTest(m_depthTestEnabled);
    m_renderer->SetBlendMode(BlendMode::Alpha);
    
    // Render segment geometry
    RenderSegmentGeometry(segment, depth, color);
}

void TubeRenderer::RenderSegmentHighlight(uint8_t segment, float depth, const glm::vec4& color) {
    if (!m_initialized || !m_renderer) return;
    
    // Render highlighted segment with enhanced color
    glm::vec4 highlightColor = color;
    highlightColor.a = 1.0f; // Full opacity for highlight
    highlightColor.x *= 1.5f; // Brighten the color
    highlightColor.y *= 1.5f;
    highlightColor.z *= 1.5f;
    
    RenderSegment(segment, depth, highlightColor);
}

void TubeRenderer::SetCameraPosition(const glm::vec3& position) {
    m_cameraPosition = position;
    UpdateMatrices();
}

void TubeRenderer::SetCameraTarget(const glm::vec3& target) {
    m_cameraTarget = target;
    UpdateMatrices();
}

void TubeRenderer::SetProjectionMatrix(const glm::mat4& projection) {
    m_projectionMatrix = projection;
}

void TubeRenderer::SetViewMatrix(const glm::mat4& view) {
    m_viewMatrix = view;
}

void TubeRenderer::SetTubeRadius(float radius) {
    m_tubeRadius = radius;
}

void TubeRenderer::SetTubeDepth(float depth) {
    m_tubeDepth = depth;
}

void TubeRenderer::SetSegmentCount(uint8_t segments) {
    m_segmentCount = segments;
}

void TubeRenderer::SetDepthLevels(uint8_t levels) {
    m_depthLevels = levels;
}

void TubeRenderer::EnableWireframe(bool enable) {
    m_wireframeEnabled = enable;
}

void TubeRenderer::EnableDepthTest(bool enable) {
    m_depthTestEnabled = enable;
}

void TubeRenderer::EnableBlending(bool enable) {
    m_blendingEnabled = enable;
}

void TubeRenderer::SetWireframeColor(const glm::vec4& color) {
    m_wireframeColor = color;
}

void TubeRenderer::SetSegmentColors(const std::vector<glm::vec4>& colors) {
    m_segmentColors = colors;
}

glm::vec3 TubeRenderer::GetSegmentPosition(uint8_t segment, float depth) const {
    float angle = static_cast<float>(segment) * (2.0f * 3.14159f / m_segmentCount);
    float scale = GetDepthScale(depth);
    
    return glm::vec3(
        cos(angle) * m_tubeRadius * scale,
        depth,
        sin(angle) * m_tubeRadius * scale
    );
}

glm::vec3 TubeRenderer::GetSegmentNormal(uint8_t segment) const {
    float angle = static_cast<float>(segment) * (2.0f * 3.14159f / m_segmentCount);
    return glm::vec3(cos(angle), 0.0f, sin(angle));
}

bool TubeRenderer::IsSegmentVisible(uint8_t segment, float depth) const {
    glm::vec3 segmentPos = GetSegmentPosition(segment, depth);
    glm::vec3 cameraPos = m_cameraPosition;
    
    // Simple visibility check
    return glm::length(segmentPos - cameraPos) < m_tubeDepth * 2.0f;
}

float TubeRenderer::GetDepthScale(float depth) const {
    // Perspective scaling based on camera distance
    return m_cameraPosition.y / (m_cameraPosition.y + depth);
}

void TubeRenderer::CreateShaders() {
    // Create tube shader
    const std::string tubeVertexShader = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aNormal;
        layout(location = 2) in vec2 aTexCoord;
        layout(location = 3) in vec4 aColor;
        
        uniform mat4 uProjection;
        uniform mat4 uView;
        uniform mat4 uModel;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoord;
        out vec4 Color;
        
        void main() {
            FragPos = vec3(uModel * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(uModel))) * aNormal;
            TexCoord = aTexCoord;
            Color = aColor;
            
            gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
        }
    )";
    
    const std::string tubeFragmentShader = R"(
        #version 330 core
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoord;
        in vec4 Color;
        
        out vec4 FragColor;
        
        void main() {
            vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
            float diff = max(dot(normalize(Normal), lightDir), 0.0);
            vec3 diffuse = diff * Color.rgb;
            vec3 ambient = 0.3 * Color.rgb;
            
            FragColor = vec4(ambient + diffuse, Color.a);
        }
    )";
    
    m_tubeShader = std::make_unique<Shader>();
    m_tubeShader->Create(tubeVertexShader, tubeFragmentShader, m_renderer->GetGraphicsAPI());
    
    // Create wireframe shader (simpler)
    const std::string wireframeVertexShader = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        
        uniform mat4 uProjection;
        uniform mat4 uView;
        uniform mat4 uModel;
        uniform vec4 uColor;
        
        out vec4 Color;
        
        void main() {
            Color = uColor;
            gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
        }
    )";
    
    const std::string wireframeFragmentShader = R"(
        #version 330 core
        in vec4 Color;
        out vec4 FragColor;
        
        void main() {
            FragColor = Color;
        }
    )";
    
    m_wireframeShader = std::make_unique<Shader>();
    m_wireframeShader->Create(wireframeVertexShader, wireframeFragmentShader, m_renderer->GetGraphicsAPI());
    
    // Create segment shader
    m_segmentShader = std::make_unique<Shader>();
    m_segmentShader->Create(tubeVertexShader, tubeFragmentShader, m_renderer->GetGraphicsAPI());
}

void TubeRenderer::CreateMeshes() {
    // Create basic tube mesh
    m_tubeMesh = std::make_unique<Mesh>();
    
    // Create wireframe mesh
    m_wireframeMesh = std::make_unique<Mesh>();
    
    // Create segment mesh
    m_segmentMesh = std::make_unique<Mesh>();
}

void TubeRenderer::UpdateMatrices() {
    m_viewMatrix = glm::lookAt(m_cameraPosition, m_cameraTarget, glm::vec3(0.0f, 0.0f, 1.0f));
}

void TubeRenderer::RenderTubeGeometry(const TubeGeometry& geometry) {
    if (!m_tubeShader || !m_tubeMesh) return;
    
    // Generate vertices from geometry
    GenerateTubeVertices(geometry);
    
    // Set shader uniforms
    SetTubeUniforms(*m_tubeShader, geometry);
    
    // Render using the renderer
    if (m_renderer) {
        m_renderer->DrawMesh(*m_tubeMesh, *m_tubeShader);
    }
}

void TubeRenderer::RenderWireframeGeometry(const TubeGeometry& geometry) {
    if (!m_wireframeShader || !m_wireframeMesh) return;
    
    // Generate wireframe vertices
    GenerateWireframeVertices(geometry);
    
    // Set shader uniforms
    SetWireframeUniforms(*m_wireframeShader);
    
    // Render using the renderer
    if (m_renderer) {
        m_renderer->DrawMesh(*m_wireframeMesh, *m_wireframeShader);
    }
}

void TubeRenderer::RenderSegmentGeometry(uint8_t segment, float depth, const glm::vec4& color) {
    if (!m_segmentShader || !m_segmentMesh) return;
    
    // Generate segment vertices
    GenerateSegmentVertices(segment, depth);
    
    // Set shader uniforms
    SetSegmentUniforms(*m_segmentShader, segment, depth, color);
    
    // Render using the renderer
    if (m_renderer) {
        m_renderer->DrawMesh(*m_segmentMesh, *m_segmentShader);
    }
}

void TubeRenderer::GenerateTubeVertices(const TubeGeometry& geometry) {
    // This would generate vertices from the TubeGeometry
    // For now, we'll use a placeholder implementation
    spdlog::debug("Generating tube vertices for {} vertices", geometry.getVertexCount());
}

void TubeRenderer::GenerateWireframeVertices(const TubeGeometry& geometry) {
    // This would generate wireframe vertices
    spdlog::debug("Generating wireframe vertices");
}

void TubeRenderer::GenerateSegmentVertices(uint8_t segment, float depth) {
    // This would generate vertices for a specific segment
    spdlog::debug("Generating segment vertices for segment {} at depth {}", segment, depth);
}

void TubeRenderer::SetShaderUniforms(Shader& shader) {
    shader.SetMat4("uProjection", m_projectionMatrix);
    shader.SetMat4("uView", m_viewMatrix);
    shader.SetMat4("uModel", m_modelMatrix);
}

void TubeRenderer::SetTubeUniforms(Shader& shader, const TubeGeometry& geometry) {
    SetShaderUniforms(shader);
    // Add tube-specific uniforms here
}

void TubeRenderer::SetWireframeUniforms(Shader& shader) {
    SetShaderUniforms(shader);
    shader.SetVec4("uColor", m_wireframeColor);
}

void TubeRenderer::SetSegmentUniforms(Shader& shader, uint8_t segment, float depth, const glm::vec4& color) {
    SetShaderUniforms(shader);
    shader.SetVec4("uColor", color);
    shader.SetFloat("uSegment", static_cast<float>(segment));
    shader.SetFloat("uDepth", depth);
}

} // namespace Tempest 