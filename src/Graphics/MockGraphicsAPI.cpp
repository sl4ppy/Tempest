#include "MockGraphicsAPI.h"
#include <spdlog/spdlog.h>

namespace Tempest {

MockGraphicsAPI::MockGraphicsAPI() 
    : m_initialized(false), m_windowCreated(false), m_shouldClose(false),
      m_clearColor(Color::Black()), m_depthTestEnabled(true), m_blendMode(BlendMode::Alpha),
      m_viewportX(0), m_viewportY(0), m_viewportWidth(1920), m_viewportHeight(1080),
      m_currentProgram(0), m_currentVAO(0), m_currentVBO(0), m_currentEBO(0), m_currentFBO(0),
      m_nextBufferId(1), m_nextShaderId(1), m_nextProgramId(1), m_nextTextureId(1), 
      m_nextFrameBufferId(1), m_nextVAOId(1) {
    spdlog::debug("MockGraphicsAPI created");
}

MockGraphicsAPI::~MockGraphicsAPI() {
    Shutdown();
    spdlog::debug("MockGraphicsAPI destroyed");
}

bool MockGraphicsAPI::Initialize(const GraphicsConfig& config) {
    IncrementCallCount("Initialize");
    m_config = config;
    m_initialized = true;
    spdlog::info("MockGraphicsAPI initialized successfully (Mock mode - no actual OpenGL)");
    return true;
}

void MockGraphicsAPI::Shutdown() {
    IncrementCallCount("Shutdown");
    if (m_initialized) {
        m_initialized = false;
        m_windowCreated = false;
        spdlog::info("MockGraphicsAPI shutdown complete");
    }
}

void MockGraphicsAPI::CreateWindow(const WindowConfig& config) {
    IncrementCallCount("CreateWindow");
    m_windowConfig = config;
    m_windowCreated = true;
    m_shouldClose = false;
    spdlog::info("Mock window created: {}x{}", config.width, config.height);
}

void MockGraphicsAPI::DestroyWindow() {
    IncrementCallCount("DestroyWindow");
    m_windowCreated = false;
    spdlog::info("Mock window destroyed");
}

bool MockGraphicsAPI::ShouldClose() const {
    IncrementCallCount("ShouldClose");
    return m_shouldClose;
}

void MockGraphicsAPI::SwapBuffers() {
    IncrementCallCount("SwapBuffers");
    // Mock buffer swapping - no-op
}

void MockGraphicsAPI::PollEvents() {
    IncrementCallCount("PollEvents");
    // Mock event polling - no-op
}

void MockGraphicsAPI::MakeCurrent() {
    IncrementCallCount("MakeCurrent");
    // Mock context management - no-op
}

void MockGraphicsAPI::Clear(const Color& color) {
    IncrementCallCount("Clear");
    m_clearColor = color;
}

void MockGraphicsAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    IncrementCallCount("SetViewport");
    m_viewportX = x;
    m_viewportY = y;
    m_viewportWidth = width;
    m_viewportHeight = height;
}

void MockGraphicsAPI::SetClearColor(const Color& color) {
    IncrementCallCount("SetClearColor");
    m_clearColor = color;
}

void MockGraphicsAPI::EnableDepthTest(bool enable) {
    IncrementCallCount("EnableDepthTest");
    m_depthTestEnabled = enable;
}

void MockGraphicsAPI::SetBlendMode(BlendMode mode) {
    IncrementCallCount("SetBlendMode");
    m_blendMode = mode;
}

uint32_t MockGraphicsAPI::CreateVertexBuffer(const void* data, size_t size) {
    IncrementCallCount("CreateVertexBuffer");
    return m_nextBufferId++;
}

uint32_t MockGraphicsAPI::CreateIndexBuffer(const void* data, size_t size) {
    IncrementCallCount("CreateIndexBuffer");
    return m_nextBufferId++;
}

void MockGraphicsAPI::UpdateVertexBuffer(uint32_t buffer, const void* data, size_t size) {
    IncrementCallCount("UpdateVertexBuffer");
    // Mock buffer update - no-op
}

void MockGraphicsAPI::UpdateIndexBuffer(uint32_t buffer, const void* data, size_t size) {
    IncrementCallCount("UpdateIndexBuffer");
    // Mock buffer update - no-op
}

void MockGraphicsAPI::BindVertexBuffer(uint32_t buffer) {
    IncrementCallCount("BindVertexBuffer");
    m_currentVBO = buffer;
}

void MockGraphicsAPI::BindIndexBuffer(uint32_t buffer) {
    IncrementCallCount("BindIndexBuffer");
    m_currentEBO = buffer;
}

void MockGraphicsAPI::DeleteBuffer(uint32_t buffer) {
    IncrementCallCount("DeleteBuffer");
    // Mock buffer deletion - no-op
}

uint32_t MockGraphicsAPI::CreateVertexArray() {
    IncrementCallCount("CreateVertexArray");
    return m_nextVAOId++;
}

void MockGraphicsAPI::BindVertexArray(uint32_t vao) {
    IncrementCallCount("BindVertexArray");
    m_currentVAO = vao;
}

void MockGraphicsAPI::SetVertexAttribute(uint32_t location, uint32_t size, uint32_t type, 
                                        bool normalized, uint32_t stride, const void* offset) {
    IncrementCallCount("SetVertexAttribute");
    // Mock vertex attribute setup - no-op
}

void MockGraphicsAPI::DeleteVertexArray(uint32_t vao) {
    IncrementCallCount("DeleteVertexArray");
    // Mock VAO deletion - no-op
}

uint32_t MockGraphicsAPI::CreateShader(uint32_t type, const std::string& source) {
    IncrementCallCount("CreateShader");
    spdlog::debug("Mock shader created (type: {}, source length: {})", type, source.length());
    return m_nextShaderId++;
}

uint32_t MockGraphicsAPI::CreateProgram(const std::vector<uint32_t>& shaders) {
    IncrementCallCount("CreateProgram");
    spdlog::debug("Mock shader program created with {} shaders", shaders.size());
    return m_nextProgramId++;
}

void MockGraphicsAPI::UseProgram(uint32_t program) {
    IncrementCallCount("UseProgram");
    m_currentProgram = program;
}

void MockGraphicsAPI::DeleteProgram(uint32_t program) {
    IncrementCallCount("DeleteProgram");
    // Mock program deletion - no-op
}

void MockGraphicsAPI::DeleteShader(uint32_t shader) {
    IncrementCallCount("DeleteShader");
    // Mock shader deletion - no-op
}

void MockGraphicsAPI::SetUniform(uint32_t program, const std::string& name, int value) {
    IncrementCallCount("SetUniform");
    // Mock uniform setting - no-op
}

void MockGraphicsAPI::SetUniform(uint32_t program, const std::string& name, float value) {
    IncrementCallCount("SetUniform");
    // Mock uniform setting - no-op
}

void MockGraphicsAPI::SetUniform(uint32_t program, const std::string& name, const glm::vec2& value) {
    IncrementCallCount("SetUniform");
    // Mock uniform setting - no-op
}

void MockGraphicsAPI::SetUniform(uint32_t program, const std::string& name, const glm::vec3& value) {
    IncrementCallCount("SetUniform");
    // Mock uniform setting - no-op
}

void MockGraphicsAPI::SetUniform(uint32_t program, const std::string& name, const glm::vec4& value) {
    IncrementCallCount("SetUniform");
    // Mock uniform setting - no-op
}

void MockGraphicsAPI::SetUniform(uint32_t program, const std::string& name, const glm::mat3& value) {
    IncrementCallCount("SetUniform");
    // Mock uniform setting - no-op
}

void MockGraphicsAPI::SetUniform(uint32_t program, const std::string& name, const glm::mat4& value) {
    IncrementCallCount("SetUniform");
    // Mock uniform setting - no-op
}

void MockGraphicsAPI::DrawArrays(uint32_t mode, uint32_t first, uint32_t count) {
    IncrementCallCount("DrawArrays");
    // Mock drawing - no-op
}

void MockGraphicsAPI::DrawElements(uint32_t mode, uint32_t count, uint32_t type, const void* offset) {
    IncrementCallCount("DrawElements");
    // Mock drawing - no-op
}

uint32_t MockGraphicsAPI::CreateFrameBuffer() {
    IncrementCallCount("CreateFrameBuffer");
    return m_nextFrameBufferId++;
}

void MockGraphicsAPI::BindFrameBuffer(uint32_t fbo) {
    IncrementCallCount("BindFrameBuffer");
    m_currentFBO = fbo;
}

void MockGraphicsAPI::AttachTextureToFrameBuffer(uint32_t attachment, uint32_t texture) {
    IncrementCallCount("AttachTextureToFrameBuffer");
    // Mock texture attachment - no-op
}

bool MockGraphicsAPI::IsFrameBufferComplete() {
    IncrementCallCount("IsFrameBufferComplete");
    return true; // Always complete in mock mode
}

void MockGraphicsAPI::DeleteFrameBuffer(uint32_t fbo) {
    IncrementCallCount("DeleteFrameBuffer");
    // Mock framebuffer deletion - no-op
}

uint32_t MockGraphicsAPI::CreateTexture() {
    IncrementCallCount("CreateTexture");
    return m_nextTextureId++;
}

void MockGraphicsAPI::BindTexture(uint32_t target, uint32_t texture) {
    IncrementCallCount("BindTexture");
    // Mock texture binding - no-op
}

void MockGraphicsAPI::SetTextureData(uint32_t target, uint32_t level, uint32_t internalFormat,
                                    uint32_t width, uint32_t height, uint32_t format, 
                                    uint32_t type, const void* data) {
    IncrementCallCount("SetTextureData");
    spdlog::debug("Mock texture data set: {}x{}", width, height);
}

void MockGraphicsAPI::SetTextureParameter(uint32_t target, uint32_t pname, int param) {
    IncrementCallCount("SetTextureParameter");
    // Mock texture parameter setting - no-op
}

void MockGraphicsAPI::DeleteTexture(uint32_t texture) {
    IncrementCallCount("DeleteTexture");
    // Mock texture deletion - no-op
}

bool MockGraphicsAPI::CheckError() {
    IncrementCallCount("CheckError");
    return false; // No errors in mock mode
}

std::string MockGraphicsAPI::GetErrorString() {
    IncrementCallCount("GetErrorString");
    return ""; // No errors in mock mode
}

uint32_t MockGraphicsAPI::GetCallCount(const std::string& functionName) const {
    auto it = m_callCounts.find(functionName);
    return (it != m_callCounts.end()) ? it->second : 0;
}

void MockGraphicsAPI::ResetCallCounts() {
    m_callCounts.clear();
}

void MockGraphicsAPI::IncrementCallCount(const std::string& functionName) const {
    m_callCounts[functionName]++;
}

} // namespace Tempest 