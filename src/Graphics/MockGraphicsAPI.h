#pragma once

#include "GraphicsAPI.h"
#include <spdlog/spdlog.h>
#include <unordered_map>

namespace Tempest {

/**
 * @brief Mock Graphics API for testing without OpenGL context
 * 
 * This class implements the IGraphicsAPI interface but doesn't make any actual
 * OpenGL calls. It's designed for unit testing where a full graphics context
 * is not available or needed.
 */
class MockGraphicsAPI : public IGraphicsAPI {
public:
    MockGraphicsAPI();
    ~MockGraphicsAPI() override;
    
    // Initialization
    bool Initialize(const GraphicsConfig& config) override;
    void Shutdown() override;
    
    // Window management
    void CreateWindow(const WindowConfig& config) override;
    void DestroyWindow() override;
    bool ShouldClose() const override;
    void SwapBuffers() override;
    void PollEvents() override;
    
    // Context management
    void MakeCurrent() override;
    void Clear(const Color& color) override;
    void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void SetClearColor(const Color& color) override;
    void EnableDepthTest(bool enable) override;
    void SetBlendMode(BlendMode mode) override;
    
    // Buffer management
    uint32_t CreateVertexBuffer(const void* data, size_t size) override;
    uint32_t CreateIndexBuffer(const void* data, size_t size) override;
    void UpdateVertexBuffer(uint32_t buffer, const void* data, size_t size) override;
    void UpdateIndexBuffer(uint32_t buffer, const void* data, size_t size) override;
    void BindVertexBuffer(uint32_t buffer) override;
    void BindIndexBuffer(uint32_t buffer) override;
    void DeleteBuffer(uint32_t buffer) override;
    
    // Vertex array management
    uint32_t CreateVertexArray() override;
    void BindVertexArray(uint32_t vao) override;
    void SetVertexAttribute(uint32_t location, uint32_t size, uint32_t type, 
                          bool normalized, uint32_t stride, const void* offset) override;
    void DeleteVertexArray(uint32_t vao) override;
    
    // Shader management
    uint32_t CreateShader(uint32_t type, const std::string& source) override;
    uint32_t CreateProgram(const std::vector<uint32_t>& shaders) override;
    void UseProgram(uint32_t program) override;
    void DeleteProgram(uint32_t program) override;
    void DeleteShader(uint32_t shader) override;
    
    // Uniform management
    void SetUniform(uint32_t program, const std::string& name, int value) override;
    void SetUniform(uint32_t program, const std::string& name, float value) override;
    void SetUniform(uint32_t program, const std::string& name, const glm::vec2& value) override;
    void SetUniform(uint32_t program, const std::string& name, const glm::vec3& value) override;
    void SetUniform(uint32_t program, const std::string& name, const glm::vec4& value) override;
    void SetUniform(uint32_t program, const std::string& name, const glm::mat3& value) override;
    void SetUniform(uint32_t program, const std::string& name, const glm::mat4& value) override;
    
    // Drawing
    void DrawArrays(uint32_t mode, uint32_t first, uint32_t count) override;
    void DrawElements(uint32_t mode, uint32_t count, uint32_t type, const void* offset) override;
    
    // Frame buffer management
    uint32_t CreateFrameBuffer() override;
    void BindFrameBuffer(uint32_t fbo) override;
    void AttachTextureToFrameBuffer(uint32_t attachment, uint32_t texture) override;
    bool IsFrameBufferComplete() override;
    void DeleteFrameBuffer(uint32_t fbo) override;
    
    // Texture management
    uint32_t CreateTexture() override;
    void BindTexture(uint32_t target, uint32_t texture) override;
    void SetTextureData(uint32_t target, uint32_t level, uint32_t internalFormat,
                       uint32_t width, uint32_t height, uint32_t format, 
                       uint32_t type, const void* data) override;
    void SetTextureParameter(uint32_t target, uint32_t pname, int param) override;
    void DeleteTexture(uint32_t texture) override;
    
    // Error checking
    bool CheckError() override;
    std::string GetErrorString() override;
    
    // Mock-specific methods for testing
    bool IsInitialized() const { return m_initialized; }
    uint32_t GetCallCount(const std::string& functionName) const;
    void ResetCallCounts();
    
private:
    bool m_initialized;
    bool m_windowCreated;
    bool m_shouldClose;
    GraphicsConfig m_config;
    WindowConfig m_windowConfig;
    
    // Mock state tracking
    Color m_clearColor;
    bool m_depthTestEnabled;
    BlendMode m_blendMode;
    uint32_t m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight;
    uint32_t m_currentProgram;
    uint32_t m_currentVAO;
    uint32_t m_currentVBO;
    uint32_t m_currentEBO;
    uint32_t m_currentFBO;
    
    // Resource ID generators
    uint32_t m_nextBufferId;
    uint32_t m_nextShaderId;
    uint32_t m_nextProgramId;
    uint32_t m_nextTextureId;
    uint32_t m_nextFrameBufferId;
    uint32_t m_nextVAOId;
    
    // Call counting for testing
    mutable std::unordered_map<std::string, uint32_t> m_callCounts;
    
    void IncrementCallCount(const std::string& functionName) const;
};

} // namespace Tempest 