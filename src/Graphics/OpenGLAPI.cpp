#ifdef USE_SDL
#include <SDL.h>
#endif
#include "OpenGLAPI.h"
#include "glad/gl.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Tempest {

OpenGLAPI::OpenGLAPI() 
    : m_window(nullptr), m_initialized(false), m_currentProgram(0), 
      m_currentVAO(0), m_currentVBO(0), m_currentEBO(0), m_currentFBO(0) {
}

OpenGLAPI::~OpenGLAPI() {
    Shutdown();
}

bool OpenGLAPI::Initialize(const GraphicsConfig& config) {
    m_config = config;
    
    // Check if OpenGL context is already available (from SDL)
    // For now, just assume OpenGL is available since SDL created the context
    spdlog::info("OpenGL context assumed to be available from SDL");
    
    if (m_config.enableDebug) {
        SetupDebugCallback();
    }
    
    // Setup default OpenGL state
    SetupDefaultState();
    
    // Get real OpenGL version information
    const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    
    spdlog::info("OpenGL Version: {}", version ? version : "Unknown");
    spdlog::info("OpenGL Renderer: {}", renderer ? renderer : "Unknown");
    spdlog::info("OpenGL Vendor: {}", vendor ? vendor : "Unknown");
    
    m_initialized = true;
    spdlog::info("OpenGL API initialized successfully");
    
    return true;
}

void OpenGLAPI::Shutdown() {
    // Don't destroy window or terminate GLFW since we're using SDL
    m_initialized = false;
}

void OpenGLAPI::CreateWindow(const WindowConfig& config) {
    // Window is created by SDL, not here
    spdlog::info("Window creation handled by SDL");
}

void OpenGLAPI::DestroyWindow() {
    // Window destruction handled by SDL
    spdlog::info("Window destruction handled by SDL");
}

bool OpenGLAPI::ShouldClose() const {
    // This should be handled by SDL event polling
    return false;
}

void OpenGLAPI::SwapBuffers() {
    // Buffer swapping handled by SDL
    // SDL_GL_SwapWindow(window) should be called from main
}

void OpenGLAPI::PollEvents() {
    // Event polling handled by SDL
}

void OpenGLAPI::MakeCurrent() {
    // Context management handled by SDL
}

void OpenGLAPI::Clear(const Color& color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    glViewport(static_cast<GLint>(x), static_cast<GLint>(y), 
               static_cast<GLsizei>(width), static_cast<GLsizei>(height));
}

void OpenGLAPI::SetClearColor(const Color& color) {
    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLAPI::EnableDepthTest(bool enable) {
    if (enable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void OpenGLAPI::SetBlendMode(BlendMode mode) {
    switch (mode) {
        case BlendMode::None:
            glDisable(GL_BLEND);
            break;
        case BlendMode::Alpha:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendMode::Additive:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        case BlendMode::Multiplicative:
            glEnable(GL_BLEND);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;
    }
}

uint32_t OpenGLAPI::CreateVertexBuffer(const void* data, size_t size) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
    return vbo;
}

uint32_t OpenGLAPI::CreateIndexBuffer(const void* data, size_t size) {
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
    return ebo;
}

void OpenGLAPI::UpdateVertexBuffer(uint32_t buffer, const void* data, size_t size) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), data);
}

void OpenGLAPI::UpdateIndexBuffer(uint32_t buffer, const void* data, size_t size) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), data);
}

void OpenGLAPI::BindVertexBuffer(uint32_t buffer) {
    m_currentVBO = buffer;
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void OpenGLAPI::BindIndexBuffer(uint32_t buffer) {
    m_currentEBO = buffer;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}

void OpenGLAPI::DeleteBuffer(uint32_t buffer) {
    GLuint glBuffer = static_cast<GLuint>(buffer);
    glDeleteBuffers(1, &glBuffer);
}

uint32_t OpenGLAPI::CreateVertexArray() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    return vao;
}

void OpenGLAPI::BindVertexArray(uint32_t vao) {
    m_currentVAO = vao;
    glBindVertexArray(vao);
}

void OpenGLAPI::SetVertexAttribute(uint32_t location, uint32_t size, uint32_t type, 
                                  bool normalized, uint32_t stride, const void* offset) {
    glVertexAttribPointer(location, static_cast<GLint>(size), static_cast<GLenum>(type), 
                         normalized ? GL_TRUE : GL_FALSE, static_cast<GLsizei>(stride), offset);
    glEnableVertexAttribArray(location);
}

void OpenGLAPI::DeleteVertexArray(uint32_t vao) {
    GLuint glVao = static_cast<GLuint>(vao);
    glDeleteVertexArrays(1, &glVao);
}

uint32_t OpenGLAPI::CreateShader(uint32_t type, const std::string& source) {
    GLuint shader = glCreateShader(static_cast<GLenum>(type));
    const char* sourceStr = source.c_str();
    glShaderSource(shader, 1, &sourceStr, nullptr);
    glCompileShader(shader);
    
    // Check compilation status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        spdlog::error("Shader compilation failed: {}", infoLog);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

uint32_t OpenGLAPI::CreateProgram(const std::vector<uint32_t>& shaders) {
    GLuint program = glCreateProgram();
    
    for (uint32_t shader : shaders) {
        glAttachShader(program, shader);
    }
    
    glLinkProgram(program);
    
    // Check linking status
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        spdlog::error("Program linking failed: {}", infoLog);
        glDeleteProgram(program);
        return 0;
    }
    
    // Detach shaders after linking
    for (uint32_t shader : shaders) {
        glDetachShader(program, shader);
    }
    
    return program;
}

void OpenGLAPI::UseProgram(uint32_t program) {
    m_currentProgram = program;
    glUseProgram(program);
}

void OpenGLAPI::DeleteProgram(uint32_t program) {
    GLuint glProgram = static_cast<GLuint>(program);
    glDeleteProgram(glProgram);
}

void OpenGLAPI::DeleteShader(uint32_t shader) {
    GLuint glShader = static_cast<GLuint>(shader);
    glDeleteShader(glShader);
}

void OpenGLAPI::SetUniform(uint32_t program, const std::string& name, int value) {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void OpenGLAPI::SetUniform(uint32_t program, const std::string& name, float value) {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void OpenGLAPI::SetUniform(uint32_t program, const std::string& name, const glm::vec2& value) {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location != -1) {
        glUniform2fv(location, 1, glm::value_ptr(value));
    }
}

void OpenGLAPI::SetUniform(uint32_t program, const std::string& name, const glm::vec3& value) {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location != -1) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void OpenGLAPI::SetUniform(uint32_t program, const std::string& name, const glm::vec4& value) {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location != -1) {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }
}

void OpenGLAPI::SetUniform(uint32_t program, const std::string& name, const glm::mat3& value) {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location != -1) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void OpenGLAPI::SetUniform(uint32_t program, const std::string& name, const glm::mat4& value) {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void OpenGLAPI::DrawArrays(uint32_t mode, uint32_t first, uint32_t count) {
    glDrawArrays(static_cast<GLenum>(mode), static_cast<GLint>(first), static_cast<GLsizei>(count));
}

void OpenGLAPI::DrawElements(uint32_t mode, uint32_t count, uint32_t type, const void* offset) {
    glDrawElements(static_cast<GLenum>(mode), static_cast<GLsizei>(count), 
                   static_cast<GLenum>(type), offset);
}

uint32_t OpenGLAPI::CreateFrameBuffer() {
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    return fbo;
}

void OpenGLAPI::BindFrameBuffer(uint32_t fbo) {
    m_currentFBO = fbo;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void OpenGLAPI::AttachTextureToFrameBuffer(uint32_t attachment, uint32_t texture) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(attachment), 
                           GL_TEXTURE_2D, texture, 0);
}

bool OpenGLAPI::IsFrameBufferComplete() {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void OpenGLAPI::DeleteFrameBuffer(uint32_t fbo) {
    GLuint glFbo = static_cast<GLuint>(fbo);
    glDeleteFramebuffers(1, &glFbo);
}

uint32_t OpenGLAPI::CreateTexture() {
    GLuint texture;
    glGenTextures(1, &texture);
    return texture;
}

void OpenGLAPI::BindTexture(uint32_t target, uint32_t texture) {
    glBindTexture(static_cast<GLenum>(target), texture);
}

void OpenGLAPI::SetTextureData(uint32_t target, uint32_t level, uint32_t internalFormat,
                               uint32_t width, uint32_t height, uint32_t format, 
                               uint32_t type, const void* data) {
    glTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level), 
                  static_cast<GLint>(internalFormat), static_cast<GLsizei>(width), 
                  static_cast<GLsizei>(height), 0, static_cast<GLenum>(format), 
                  static_cast<GLenum>(type), data);
}

void OpenGLAPI::SetTextureParameter(uint32_t target, uint32_t pname, int param) {
    glTexParameteri(static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
}

void OpenGLAPI::DeleteTexture(uint32_t texture) {
    GLuint glTexture = static_cast<GLuint>(texture);
    glDeleteTextures(1, &glTexture);
}

bool OpenGLAPI::CheckError() {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        switch (error) {
            case GL_INVALID_ENUM:
                m_lastError = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                m_lastError = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                m_lastError = "GL_INVALID_OPERATION";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                m_lastError = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                m_lastError = "GL_OUT_OF_MEMORY";
                break;
            default:
                m_lastError = "Unknown OpenGL error";
                break;
        }
        return true;
    }
    return false;
}

std::string OpenGLAPI::GetErrorString() {
    return m_lastError;
}

// Private helper methods
bool OpenGLAPI::InitializeGLFW() {
    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW");
        return false;
    }
    return true;
}

bool OpenGLAPI::InitializeGLAD() {
    // Load GLAD
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        spdlog::error("Failed to initialize GLAD");
        return false;
    }
    return true;
}

void OpenGLAPI::SetupDebugCallback() {
    // Debug callback setup - simplified for now
    // In a full implementation, we would check for GL_ARB_debug_output extension
    spdlog::info("OpenGL debug callback setup (simplified)");
}

void OpenGLAPI::SetupDefaultState() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

int OpenGLAPI::GetGLBlendMode(BlendMode mode) {
    switch (mode) {
        case BlendMode::None: return 0;
        case BlendMode::Alpha: return 1;
        case BlendMode::Additive: return 2;
        case BlendMode::Multiplicative: return 3;
        default: return 1;
    }
}

void OpenGLAPI::LogError(const std::string& message) {
    spdlog::error("OpenGL API Error: {}", message);
    m_lastError = message;
}

} // namespace Tempest 