#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace Tempest {

// Forward declarations
struct Color;
enum class BlendMode;

// Configuration structures
struct GraphicsConfig {
    std::string api = "OpenGL";  // "OpenGL" or "Vulkan"
    uint32_t majorVersion = 4;
    uint32_t minorVersion = 6;
    bool enableDebug = false;
    bool enableVSync = true;
    uint32_t maxTextureSize = 4096;
    uint32_t maxAnisotropy = 16;
};

struct WindowConfig {
    std::string title = "Tempest";
    uint32_t width = 1920;
    uint32_t height = 1080;
    bool fullscreen = false;
    bool resizable = true;
    uint32_t samples = 4;  // MSAA samples
};

// Color structure
struct Color {
    float r, g, b, a;
    
    Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
    
    static Color White() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
    static Color Black() { return Color(0.0f, 0.0f, 0.0f, 1.0f); }
    static Color Red() { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
    static Color Green() { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
    static Color Blue() { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
    static Color Yellow() { return Color(1.0f, 1.0f, 0.0f, 1.0f); }
    static Color Cyan() { return Color(0.0f, 1.0f, 1.0f, 1.0f); }
    static Color Magenta() { return Color(1.0f, 0.0f, 1.0f, 1.0f); }
};

// Blend mode enumeration
enum class BlendMode {
    None,
    Alpha,
    Additive,
    Multiplicative
};

// Graphics API abstraction layer
class IGraphicsAPI {
public:
    virtual ~IGraphicsAPI() = default;
    
    // Initialization
    virtual bool Initialize(const GraphicsConfig& config) = 0;
    virtual void Shutdown() = 0;
    
    // Window management
    virtual void CreateWindow(const WindowConfig& config) = 0;
    virtual void DestroyWindow() = 0;
    virtual bool ShouldClose() const = 0;
    virtual void SwapBuffers() = 0;
    virtual void PollEvents() = 0;
    
    // Context management
    virtual void MakeCurrent() = 0;
    virtual void Clear(const Color& color) = 0;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void SetClearColor(const Color& color) = 0;
    virtual void EnableDepthTest(bool enable) = 0;
    virtual void SetBlendMode(BlendMode mode) = 0;
    
    // Buffer management
    virtual uint32_t CreateVertexBuffer(const void* data, size_t size) = 0;
    virtual uint32_t CreateIndexBuffer(const void* data, size_t size) = 0;
    virtual void UpdateVertexBuffer(uint32_t buffer, const void* data, size_t size) = 0;
    virtual void UpdateIndexBuffer(uint32_t buffer, const void* data, size_t size) = 0;
    virtual void BindVertexBuffer(uint32_t buffer) = 0;
    virtual void BindIndexBuffer(uint32_t buffer) = 0;
    virtual void DeleteBuffer(uint32_t buffer) = 0;
    
    // Vertex array management
    virtual uint32_t CreateVertexArray() = 0;
    virtual void BindVertexArray(uint32_t vao) = 0;
    virtual void SetVertexAttribute(uint32_t location, uint32_t size, uint32_t type, 
                                  bool normalized, uint32_t stride, const void* offset) = 0;
    virtual void DeleteVertexArray(uint32_t vao) = 0;
    
    // Shader management
    virtual uint32_t CreateShader(uint32_t type, const std::string& source) = 0;
    virtual uint32_t CreateProgram(const std::vector<uint32_t>& shaders) = 0;
    virtual void UseProgram(uint32_t program) = 0;
    virtual void DeleteProgram(uint32_t program) = 0;
    virtual void DeleteShader(uint32_t shader) = 0;
    
    // Uniform management
    virtual void SetUniform(uint32_t program, const std::string& name, int value) = 0;
    virtual void SetUniform(uint32_t program, const std::string& name, float value) = 0;
    virtual void SetUniform(uint32_t program, const std::string& name, const glm::vec2& value) = 0;
    virtual void SetUniform(uint32_t program, const std::string& name, const glm::vec3& value) = 0;
    virtual void SetUniform(uint32_t program, const std::string& name, const glm::vec4& value) = 0;
    virtual void SetUniform(uint32_t program, const std::string& name, const glm::mat3& value) = 0;
    virtual void SetUniform(uint32_t program, const std::string& name, const glm::mat4& value) = 0;
    
    // Drawing
    virtual void DrawArrays(uint32_t mode, uint32_t first, uint32_t count) = 0;
    virtual void DrawElements(uint32_t mode, uint32_t count, uint32_t type, const void* offset) = 0;
    
    // Frame buffer management
    virtual uint32_t CreateFrameBuffer() = 0;
    virtual void BindFrameBuffer(uint32_t fbo) = 0;
    virtual void AttachTextureToFrameBuffer(uint32_t attachment, uint32_t texture) = 0;
    virtual bool IsFrameBufferComplete() = 0;
    virtual void DeleteFrameBuffer(uint32_t fbo) = 0;
    
    // Texture management
    virtual uint32_t CreateTexture() = 0;
    virtual void BindTexture(uint32_t target, uint32_t texture) = 0;
    virtual void SetTextureData(uint32_t target, uint32_t level, uint32_t internalFormat,
                               uint32_t width, uint32_t height, uint32_t format, 
                               uint32_t type, const void* data) = 0;
    virtual void SetTextureParameter(uint32_t target, uint32_t pname, int param) = 0;
    virtual void DeleteTexture(uint32_t texture) = 0;
    
    // Error checking
    virtual bool CheckError() = 0;
    virtual std::string GetErrorString() = 0;
};

// Factory function for creating graphics API
std::unique_ptr<IGraphicsAPI> CreateGraphicsAPI(const std::string& api);

} // namespace Tempest 