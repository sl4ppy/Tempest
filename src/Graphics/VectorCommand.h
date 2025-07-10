#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <string>

namespace Tempest {

// Vector command types based on original Tempest vsdraw system
enum class VectorCommandType {
    MoveTo,         // Move to position without drawing
    LineTo,         // Draw line to position
    Circle,         // Draw circle
    Arc,            // Draw arc
    Rectangle,      // Draw rectangle
    Triangle,       // Draw triangle
    Polygon,        // Draw polygon
    Text,           // Draw text
    Number,         // Draw number
    SetColor,       // Set drawing color
    SetThickness,   // Set line thickness
    SetStyle,       // Set line style (solid, dashed, etc.)
    BeginPath,      // Begin a new path
    EndPath,        // End current path
    Fill,           // Fill current path
    Stroke,         // Stroke current path
    Clear,          // Clear screen
    Flush           // Flush all pending commands
};

// Vector command structure
struct VectorCommand {
    VectorCommandType type;
    std::vector<glm::vec2> points;
    glm::vec4 color;
    float thickness;
    std::string text;
    uint32_t flags;
    
    VectorCommand(VectorCommandType cmdType) 
        : type(cmdType), color(1.0f, 1.0f, 1.0f, 1.0f), thickness(1.0f), flags(0) {}
};

// Vector graphics command processor
class VectorCommandProcessor {
public:
    VectorCommandProcessor();
    ~VectorCommandProcessor();
    
    // Command creation methods
    void MoveTo(float x, float y);
    void LineTo(float x, float y);
    void Circle(float x, float y, float radius);
    void Arc(float x, float y, float radius, float startAngle, float endAngle);
    void Rectangle(float x, float y, float width, float height);
    void Triangle(float x1, float y1, float x2, float y2, float x3, float y3);
    void Polygon(const std::vector<glm::vec2>& points);
    void Text(float x, float y, const std::string& text);
    void Number(float x, float y, int number);
    
    // Style commands
    void SetColor(float r, float g, float b, float a = 1.0f);
    void SetColor(const glm::vec4& color);
    void SetThickness(float thickness);
    void SetStyle(uint32_t style);
    
    // Path commands
    void BeginPath();
    void EndPath();
    void Fill();
    void Stroke();
    
    // Utility commands
    void Clear();
    void Flush();
    
    // Command processing
    void ProcessCommands();
    void ClearCommands();
    
    // Command optimization
    void OptimizeCommands();
    void SortCommandsByType();
    void MergeAdjacentLines();
    void RemoveRedundantCommands();
    
    // Batch processing
    std::vector<VectorCommand> GetCommandsBatch(size_t startIndex, size_t count);
    size_t GetCommandCount() const;
    
    // Command access
    const std::vector<VectorCommand>& GetCommands() const { return m_commands; }
    std::vector<VectorCommand>& GetCommands() { return m_commands; }
    
    // State management
    void ResetState();
    glm::vec4 GetCurrentColor() const { return m_currentColor; }
    float GetCurrentThickness() const { return m_currentThickness; }
    uint32_t GetCurrentStyle() const { return m_currentStyle; }
    
private:
    std::vector<VectorCommand> m_commands;
    glm::vec4 m_currentColor;
    float m_currentThickness;
    uint32_t m_currentStyle;
    glm::vec2 m_currentPosition;
    bool m_pathActive;
    
    void AddCommand(const VectorCommand& command);
    void UpdateCurrentPosition(float x, float y);
    bool AreCommandsAdjacent(const VectorCommand& a, const VectorCommand& b);
};

// Vector graphics renderer
class VectorRenderer {
public:
    VectorRenderer();
    ~VectorRenderer();
    
    bool Initialize();
    void Shutdown();
    
    // Rendering methods
    void RenderCommands(const std::vector<VectorCommand>& commands);
    void RenderCommand(const VectorCommand& command);
    
    // Batch rendering
    void BeginBatch();
    void EndBatch();
    void FlushBatch();
    
    // State management
    void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    void SetProjectionMatrix(const glm::mat4& projection);
    void SetViewMatrix(const glm::mat4& view);
    void SetModelMatrix(const glm::mat4& model);
    
private:
    bool m_initialized;
    std::vector<VectorCommand> m_batchCommands;
    bool m_batchActive;
    
    // OpenGL resources
    uint32_t m_lineShader;
    uint32_t m_circleShader;
    uint32_t m_textShader;
    uint32_t m_vao;
    uint32_t m_vbo;
    
    // Matrices
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_modelMatrix;
    
    void CreateShaders();
    void CreateBuffers();
    void RenderLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, float thickness);
    void RenderCircle(const glm::vec2& center, float radius, const glm::vec4& color);
    void RenderArc(const glm::vec2& center, float radius, float startAngle, float endAngle, const glm::vec4& color);
    void RenderRectangle(const glm::vec2& position, float width, float height, const glm::vec4& color, float thickness);
    void RenderTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec4& color, float thickness);
    void RenderPolygon(const std::vector<glm::vec2>& points, const glm::vec4& color, float thickness);
    void RenderText(const glm::vec2& position, const std::string& text, const glm::vec4& color);
    void RenderNumber(const glm::vec2& position, int number, const glm::vec4& color);
};

} // namespace Tempest 