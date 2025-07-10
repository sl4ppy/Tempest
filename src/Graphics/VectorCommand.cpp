#include "VectorCommand.h"
#include "glad/gl.h"
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <algorithm>

namespace Tempest {

// VectorCommandProcessor implementation
VectorCommandProcessor::VectorCommandProcessor()
    : m_currentColor(1.0f, 1.0f, 1.0f, 1.0f)
    , m_currentThickness(1.0f)
    , m_currentStyle(0)
    , m_currentPosition(0.0f, 0.0f)
    , m_pathActive(false) {
}

VectorCommandProcessor::~VectorCommandProcessor() {
    ClearCommands();
}

void VectorCommandProcessor::MoveTo(float x, float y) {
    VectorCommand command(VectorCommandType::MoveTo);
    command.points.push_back(glm::vec2(x, y));
    command.color = m_currentColor;
    command.thickness = m_currentThickness;
    AddCommand(command);
    UpdateCurrentPosition(x, y);
}

void VectorCommandProcessor::LineTo(float x, float y) {
    VectorCommand command(VectorCommandType::LineTo);
    command.points.push_back(m_currentPosition);
    command.points.push_back(glm::vec2(x, y));
    command.color = m_currentColor;
    command.thickness = m_currentThickness;
    AddCommand(command);
    UpdateCurrentPosition(x, y);
}

void VectorCommandProcessor::Circle(float x, float y, float radius) {
    VectorCommand command(VectorCommandType::Circle);
    command.points.push_back(glm::vec2(x, y));
    command.points.push_back(glm::vec2(radius, 0.0f)); // Store radius as second point
    command.color = m_currentColor;
    command.thickness = m_currentThickness;
    AddCommand(command);
}

void VectorCommandProcessor::Arc(float x, float y, float radius, float startAngle, float endAngle) {
    VectorCommand command(VectorCommandType::Arc);
    command.points.push_back(glm::vec2(x, y));
    command.points.push_back(glm::vec2(radius, startAngle));
    command.points.push_back(glm::vec2(endAngle, 0.0f));
    command.color = m_currentColor;
    command.thickness = m_currentThickness;
    AddCommand(command);
}

void VectorCommandProcessor::Rectangle(float x, float y, float width, float height) {
    VectorCommand command(VectorCommandType::Rectangle);
    command.points.push_back(glm::vec2(x, y));
    command.points.push_back(glm::vec2(width, height));
    command.color = m_currentColor;
    command.thickness = m_currentThickness;
    AddCommand(command);
}

void VectorCommandProcessor::Triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    VectorCommand command(VectorCommandType::Triangle);
    command.points.push_back(glm::vec2(x1, y1));
    command.points.push_back(glm::vec2(x2, y2));
    command.points.push_back(glm::vec2(x3, y3));
    command.color = m_currentColor;
    command.thickness = m_currentThickness;
    AddCommand(command);
}

void VectorCommandProcessor::Polygon(const std::vector<glm::vec2>& points) {
    VectorCommand command(VectorCommandType::Polygon);
    command.points = points;
    command.color = m_currentColor;
    command.thickness = m_currentThickness;
    AddCommand(command);
}

void VectorCommandProcessor::Text(float x, float y, const std::string& text) {
    VectorCommand command(VectorCommandType::Text);
    command.points.push_back(glm::vec2(x, y));
    command.text = text;
    command.color = m_currentColor;
    command.thickness = m_currentThickness;
    AddCommand(command);
}

void VectorCommandProcessor::Number(float x, float y, int number) {
    VectorCommand command(VectorCommandType::Number);
    command.points.push_back(glm::vec2(x, y));
    command.points.push_back(glm::vec2(static_cast<float>(number), 0.0f));
    command.color = m_currentColor;
    command.thickness = m_currentThickness;
    AddCommand(command);
}

void VectorCommandProcessor::SetColor(float r, float g, float b, float a) {
    m_currentColor = glm::vec4(r, g, b, a);
}

void VectorCommandProcessor::SetColor(const glm::vec4& color) {
    m_currentColor = color;
}

void VectorCommandProcessor::SetThickness(float thickness) {
    m_currentThickness = thickness;
}

void VectorCommandProcessor::SetStyle(uint32_t style) {
    m_currentStyle = style;
}

void VectorCommandProcessor::BeginPath() {
    VectorCommand command(VectorCommandType::BeginPath);
    AddCommand(command);
    m_pathActive = true;
}

void VectorCommandProcessor::EndPath() {
    VectorCommand command(VectorCommandType::EndPath);
    AddCommand(command);
    m_pathActive = false;
}

void VectorCommandProcessor::Fill() {
    VectorCommand command(VectorCommandType::Fill);
    AddCommand(command);
}

void VectorCommandProcessor::Stroke() {
    VectorCommand command(VectorCommandType::Stroke);
    AddCommand(command);
}

void VectorCommandProcessor::Clear() {
    VectorCommand command(VectorCommandType::Clear);
    AddCommand(command);
}

void VectorCommandProcessor::Flush() {
    VectorCommand command(VectorCommandType::Flush);
    AddCommand(command);
}

void VectorCommandProcessor::ProcessCommands() {
    // Process all commands in the queue
    for (const auto& command : m_commands) {
        // This will be implemented when we integrate with the renderer
        spdlog::debug("Processing vector command: {}", static_cast<int>(command.type));
    }
}

void VectorCommandProcessor::ClearCommands() {
    m_commands.clear();
}

void VectorCommandProcessor::ResetState() {
    m_currentColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_currentThickness = 1.0f;
    m_currentStyle = 0;
    m_currentPosition = glm::vec2(0.0f, 0.0f);
    m_pathActive = false;
}

// New optimization methods
void VectorCommandProcessor::OptimizeCommands() {
    if (m_commands.empty()) return;
    
    // Sort commands by type for better batching
    SortCommandsByType();
    
    // Merge adjacent lines with same properties
    MergeAdjacentLines();
    
    // Remove redundant commands
    RemoveRedundantCommands();
}

void VectorCommandProcessor::SortCommandsByType() {
    std::sort(m_commands.begin(), m_commands.end(), [](const VectorCommand& a, const VectorCommand& b) {
        return static_cast<int>(a.type) < static_cast<int>(b.type);
    });
}

void VectorCommandProcessor::MergeAdjacentLines() {
    if (m_commands.size() < 2) return;
    
    std::vector<VectorCommand> optimizedCommands;
    optimizedCommands.reserve(m_commands.size());
    
    for (const auto& command : m_commands) {
        if (command.type == VectorCommandType::LineTo && !optimizedCommands.empty()) {
            auto& lastCommand = optimizedCommands.back();
            if (lastCommand.type == VectorCommandType::LineTo && 
                AreCommandsAdjacent(lastCommand, command) &&
                lastCommand.color == command.color &&
                lastCommand.thickness == command.thickness) {
                // Merge the lines by extending the last command
                lastCommand.points[1] = command.points[1];
                continue;
            }
        }
        optimizedCommands.push_back(command);
    }
    
    m_commands = std::move(optimizedCommands);
}

void VectorCommandProcessor::RemoveRedundantCommands() {
    if (m_commands.empty()) return;
    
    std::vector<VectorCommand> filteredCommands;
    filteredCommands.reserve(m_commands.size());
    
    for (const auto& command : m_commands) {
        // Skip commands that don't actually render anything
        if (command.type == VectorCommandType::LineTo && command.points.size() >= 2) {
            const auto& start = command.points[0];
            const auto& end = command.points[1];
            // Skip zero-length lines
            if (glm::distance(start, end) < 0.001f) {
                continue;
            }
        }
        
        filteredCommands.push_back(command);
    }
    
    m_commands = std::move(filteredCommands);
}

bool VectorCommandProcessor::AreCommandsAdjacent(const VectorCommand& a, const VectorCommand& b) {
    if (a.type != VectorCommandType::LineTo || b.type != VectorCommandType::LineTo) {
        return false;
    }
    
    if (a.points.size() < 2 || b.points.size() < 2) {
        return false;
    }
    
    // Check if the end of command A matches the start of command B
    const float threshold = 0.001f;
    return glm::distance(a.points[1], b.points[0]) < threshold;
}

// Batch processing methods
std::vector<VectorCommand> VectorCommandProcessor::GetCommandsBatch(size_t startIndex, size_t count) {
    std::vector<VectorCommand> batch;
    
    if (startIndex >= m_commands.size()) {
        return batch;
    }
    
    size_t endIndex = std::min(startIndex + count, m_commands.size());
    batch.reserve(endIndex - startIndex);
    
    for (size_t i = startIndex; i < endIndex; ++i) {
        batch.push_back(m_commands[i]);
    }
    
    return batch;
}

size_t VectorCommandProcessor::GetCommandCount() const {
    return m_commands.size();
}

void VectorCommandProcessor::AddCommand(const VectorCommand& command) {
    m_commands.push_back(command);
}

void VectorCommandProcessor::UpdateCurrentPosition(float x, float y) {
    m_currentPosition = glm::vec2(x, y);
}

// VectorRenderer implementation
VectorRenderer::VectorRenderer()
    : m_initialized(false)
    , m_batchActive(false)
    , m_lineShader(0)
    , m_circleShader(0)
    , m_textShader(0)
    , m_vao(0)
    , m_vbo(0)
    , m_projectionMatrix(1.0f)
    , m_viewMatrix(1.0f)
    , m_modelMatrix(1.0f) {
}

VectorRenderer::~VectorRenderer() {
    Shutdown();
}

bool VectorRenderer::Initialize() {
    try {
        CreateShaders();
        CreateBuffers();
        
        // Enable OpenGL states for line rendering
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        
        // Set initial matrices
        m_projectionMatrix = glm::mat4(1.0f);
        m_viewMatrix = glm::mat4(1.0f);
        m_modelMatrix = glm::mat4(1.0f);
        
        m_initialized = true;
        spdlog::info("VectorRenderer initialized successfully");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("VectorRenderer initialization failed: {}", e.what());
        return false;
    }
}

void VectorRenderer::Shutdown() {
    if (!m_initialized) return;
    
    if (m_lineShader) glDeleteProgram(m_lineShader);
    if (m_circleShader) glDeleteProgram(m_circleShader);
    if (m_textShader) glDeleteProgram(m_textShader);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    
    m_initialized = false;
    spdlog::info("VectorRenderer shut down");
}

void VectorRenderer::RenderCommands(const std::vector<VectorCommand>& commands) {
    if (!m_initialized) return;
    
    for (const auto& command : commands) {
        RenderCommand(command);
    }
}

void VectorRenderer::RenderCommand(const VectorCommand& command) {
    if (!m_initialized) return;
    
    switch (command.type) {
        case VectorCommandType::MoveTo:
            // MoveTo just updates position, no rendering needed
            break;
            
        case VectorCommandType::LineTo:
            if (command.points.size() >= 2) {
                RenderLine(command.points[0], command.points[1], command.color, command.thickness);
            }
            break;
            
        case VectorCommandType::Circle:
            if (command.points.size() >= 2) {
                float radius = command.points[1].x;
                RenderCircle(command.points[0], radius, command.color);
            }
            break;
            
        case VectorCommandType::Arc:
            if (command.points.size() >= 3) {
                float radius = command.points[1].x;
                float startAngle = command.points[1].y;
                float endAngle = command.points[2].x;
                RenderArc(command.points[0], radius, startAngle, endAngle, command.color);
            }
            break;
            
        case VectorCommandType::Rectangle:
            if (command.points.size() >= 2) {
                float width = command.points[1].x;
                float height = command.points[1].y;
                RenderRectangle(command.points[0], width, height, command.color, command.thickness);
            }
            break;
            
        case VectorCommandType::Triangle:
            if (command.points.size() >= 3) {
                RenderTriangle(command.points[0], command.points[1], command.points[2], command.color, command.thickness);
            }
            break;
            
        case VectorCommandType::Polygon:
            if (command.points.size() >= 3) {
                RenderPolygon(command.points, command.color, command.thickness);
            }
            break;
            
        case VectorCommandType::Text:
            if (command.points.size() >= 1) {
                RenderText(command.points[0], command.text, command.color);
            }
            break;
            
        case VectorCommandType::Number:
            if (command.points.size() >= 2) {
                int number = static_cast<int>(command.points[1].x);
                RenderNumber(command.points[0], number, command.color);
            }
            break;
            
        default:
            spdlog::debug("Unhandled vector command type: {}", static_cast<int>(command.type));
            break;
    }
}

void VectorRenderer::BeginBatch() {
    m_batchCommands.clear();
    m_batchActive = true;
}

void VectorRenderer::EndBatch() {
    m_batchActive = false;
}

void VectorRenderer::FlushBatch() {
    if (!m_batchActive) return;
    
    RenderCommands(m_batchCommands);
    m_batchCommands.clear();
    m_batchActive = false;
}

void VectorRenderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    glViewport(static_cast<GLint>(x), static_cast<GLint>(y), 
               static_cast<GLsizei>(width), static_cast<GLsizei>(height));
}

void VectorRenderer::SetProjectionMatrix(const glm::mat4& projection) {
    m_projectionMatrix = projection;
}

void VectorRenderer::SetViewMatrix(const glm::mat4& view) {
    m_viewMatrix = view;
}

void VectorRenderer::SetModelMatrix(const glm::mat4& model) {
    m_modelMatrix = model;
}

void VectorRenderer::CreateShaders() {
    // Simple line shader
    const char* lineVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec4 aColor;
        
        uniform mat4 projection;
        uniform mat4 view;
        uniform mat4 model;
        
        out vec4 Color;
        
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
            Color = aColor;
        }
    )";
    
    const char* lineFragmentShader = R"(
        #version 330 core
        in vec4 Color;
        out vec4 FragColor;
        
        void main() {
            FragColor = Color;
        }
    )";
    
    // Create and compile vertex shader
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &lineVertexShader, nullptr);
    glCompileShader(vertexShader);
    
    // Check vertex shader compilation
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        spdlog::error("Vertex shader compilation failed: {}", infoLog);
        glDeleteShader(vertexShader);
        throw std::runtime_error("Vertex shader compilation failed");
    }
    
    // Create and compile fragment shader
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &lineFragmentShader, nullptr);
    glCompileShader(fragmentShader);
    
    // Check fragment shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        spdlog::error("Fragment shader compilation failed: {}", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        throw std::runtime_error("Fragment shader compilation failed");
    }
    
    // Create program
    m_lineShader = glCreateProgram();
    glAttachShader(m_lineShader, vertexShader);
    glAttachShader(m_lineShader, fragmentShader);
    glLinkProgram(m_lineShader);
    
    // Check program linking
    glGetProgramiv(m_lineShader, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(m_lineShader, 512, nullptr, infoLog);
        spdlog::error("Shader program linking failed: {}", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_lineShader);
        throw std::runtime_error("Shader program linking failed");
    }
    
    // Cleanup
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // For now, use the same shader for all types
    m_circleShader = m_lineShader;
    m_textShader = m_lineShader;
    
    spdlog::info("Vector shaders created successfully");
}

void VectorRenderer::CreateBuffers() {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    
    // Check if resources were created successfully
    if (m_vao == 0 || m_vbo == 0) {
        spdlog::error("Failed to create OpenGL buffers");
        throw std::runtime_error("Failed to create OpenGL buffers");
    }
    
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    spdlog::info("Vector buffers created successfully");
}

void VectorRenderer::RenderLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, float thickness) {
    if (!m_initialized) return;
    
    // Enable proper OpenGL state for line rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);  // Disable depth testing for 2D lines
    
    // Create line vertices
    std::vector<float> vertices = {
        start.x, start.y, color.r, color.g, color.b, color.a,
        end.x, end.y, color.r, color.g, color.b, color.a
    };
    
    glUseProgram(m_lineShader);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    
    // Set uniforms
    GLint projectionLoc = glGetUniformLocation(m_lineShader, "projection");
    GLint viewLoc = glGetUniformLocation(m_lineShader, "view");
    GLint modelLoc = glGetUniformLocation(m_lineShader, "model");
    
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_viewMatrix));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));
    
    // Use a minimum line width and clamp to reasonable values
    float lineWidth = std::max(1.0f, std::min(thickness, 10.0f));
    glLineWidth(lineWidth);
    
    // Draw the line
    glDrawArrays(GL_LINES, 0, 2);
    
    // Reset line width
    glLineWidth(1.0f);
    
    glBindVertexArray(0);
}

void VectorRenderer::RenderCircle(const glm::vec2& center, float radius, const glm::vec4& color) {
    if (!m_initialized) return;
    
    // For now, render as a simple line circle approximation
    const int segments = 32;
    std::vector<float> vertices;
    
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.14159f * static_cast<float>(i) / segments;
        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);
        
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
        vertices.push_back(color.a);
    }
    
    glUseProgram(m_circleShader);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    
    // Set uniforms
    GLint projectionLoc = glGetUniformLocation(m_circleShader, "projection");
    GLint viewLoc = glGetUniformLocation(m_circleShader, "view");
    GLint modelLoc = glGetUniformLocation(m_circleShader, "model");
    
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_viewMatrix));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));
    
    glDrawArrays(GL_LINE_LOOP, 0, segments + 1);
    
    glBindVertexArray(0);
}

void VectorRenderer::RenderText(const glm::vec2& position, const std::string& text, const glm::vec4& color) {
    if (!m_initialized) return;
    
    // For now, render text as simple lines (placeholder)
    // In a real implementation, this would use a font texture or vector font
    spdlog::debug("Rendering text: '{}' at ({}, {})", text, position.x, position.y);
    
    // Placeholder: render a simple rectangle for text
    float charWidth = 8.0f;
    float charHeight = 12.0f;
    
    for (size_t i = 0; i < text.length(); ++i) {
        float x = position.x + i * charWidth;
        float y = position.y;
        
        // Simple rectangle for each character
        std::vector<float> vertices = {
            x, y, color.r, color.g, color.b, color.a,
            x + charWidth, y, color.r, color.g, color.b, color.a,
            x, y + charHeight, color.r, color.g, color.b, color.a,
            x + charWidth, y + charHeight, color.r, color.g, color.b, color.a
        };
        
        glUseProgram(m_textShader);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        
        // Set uniforms
        GLint projectionLoc = glGetUniformLocation(m_textShader, "projection");
        GLint viewLoc = glGetUniformLocation(m_textShader, "view");
        GLint modelLoc = glGetUniformLocation(m_textShader, "model");
        
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_viewMatrix));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        glBindVertexArray(0);
    }
}

void VectorRenderer::RenderArc(const glm::vec2& center, float radius, float startAngle, float endAngle, const glm::vec4& color) {
    if (!m_initialized) return;
    
    // Render arc as line segments
    const int segments = 32;
    float angleDiff = endAngle - startAngle;
    
    // Ensure we don't have too many segments for small arcs
    int actualSegments = std::max(1, static_cast<int>(segments * std::abs(angleDiff) / (2.0f * 3.14159f)));
    
    std::vector<float> vertices;
    
    for (int i = 0; i <= actualSegments; ++i) {
        float angle = startAngle + (angleDiff * static_cast<float>(i) / actualSegments);
        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);
        
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
        vertices.push_back(color.a);
    }
    
    glUseProgram(m_lineShader);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    
    // Set uniforms
    GLint projectionLoc = glGetUniformLocation(m_lineShader, "projection");
    GLint viewLoc = glGetUniformLocation(m_lineShader, "view");
    GLint modelLoc = glGetUniformLocation(m_lineShader, "model");
    
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_viewMatrix));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));
    
    glDrawArrays(GL_LINE_STRIP, 0, actualSegments + 1);
    
    glBindVertexArray(0);
}

void VectorRenderer::RenderRectangle(const glm::vec2& position, float width, float height, const glm::vec4& color, float thickness) {
    if (!m_initialized) return;
    
    // Render rectangle as four lines
    glm::vec2 corners[4] = {
        glm::vec2(position.x, position.y),
        glm::vec2(position.x + width, position.y),
        glm::vec2(position.x + width, position.y + height),
        glm::vec2(position.x, position.y + height)
    };
    
    // Draw each edge
    for (int i = 0; i < 4; ++i) {
        int next = (i + 1) % 4;
        RenderLine(corners[i], corners[next], color, thickness);
    }
}

void VectorRenderer::RenderTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec4& color, float thickness) {
    if (!m_initialized) return;
    
    // Render triangle as three lines
    RenderLine(p1, p2, color, thickness);
    RenderLine(p2, p3, color, thickness);
    RenderLine(p3, p1, color, thickness);
}

void VectorRenderer::RenderPolygon(const std::vector<glm::vec2>& points, const glm::vec4& color, float thickness) {
    if (!m_initialized || points.size() < 3) return;
    
    // Render polygon as connected lines
    for (size_t i = 0; i < points.size(); ++i) {
        size_t next = (i + 1) % points.size();
        RenderLine(points[i], points[next], color, thickness);
    }
}

void VectorRenderer::RenderNumber(const glm::vec2& position, int number, const glm::vec4& color) {
    if (!m_initialized) return;
    
    // Convert number to string and render as text
    std::string numberStr = std::to_string(number);
    RenderText(position, numberStr, color);
}

} // namespace Tempest 