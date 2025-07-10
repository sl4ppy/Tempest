#pragma once

#include "GraphicsAPI.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "ShaderManager.h"

namespace Tempest {

// Forward declarations
class Camera;

// Render state structure
struct RenderState {
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
    Color clearColor;
    bool depthTestEnabled;
    BlendMode blendMode;
    uint32_t viewportX, viewportY, viewportWidth, viewportHeight;
};

// Render command structure
struct RenderCommand {
    enum class Type {
        Clear,
        SetViewport,
        SetBlendMode,
        EnableDepthTest,
        DrawMesh,
        DrawLine,
        DrawPoint,
        SetShader,
        SetTexture,
        SetUniform
    };
    
    Type type;
    std::vector<uint8_t> data;  // Command-specific data
    
    template<typename T>
    void SetData(const T& value) {
        data.resize(sizeof(T));
        std::memcpy(data.data(), &value, sizeof(T));
    }
    
    template<typename T>
    T GetData() const {
        T value;
        std::memcpy(&value, data.data(), sizeof(T));
        return value;
    }
};

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    // Initialization
    bool Initialize(const GraphicsConfig& config = GraphicsConfig{});
    void Shutdown();
    
    // Window management
    void CreateWindow(const WindowConfig& config = WindowConfig{});
    void DestroyWindow();
    bool ShouldClose() const;
    void SwapBuffers();
    void PollEvents();
    
    // Rendering pipeline
    void BeginFrame();
    void EndFrame();
    void Clear(const Color& color = Color::Black());
    void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    void SetClearColor(const Color& color);
    
    // State management
    void SetProjectionMatrix(const glm::mat4& projection);
    void SetViewMatrix(const glm::mat4& view);
    void SetModelMatrix(const glm::mat4& model);
    void SetBlendMode(BlendMode mode);
    void EnableDepthTest(bool enable);
    
    // Matrix getters
    const glm::mat4& GetProjectionMatrix() const { return m_renderState.projectionMatrix; }
    const glm::mat4& GetViewMatrix() const { return m_renderState.viewMatrix; }
    const glm::mat4& GetModelMatrix() const { return m_renderState.modelMatrix; }
    
    // Drawing commands
    void DrawMesh(const Mesh& mesh, const Shader& shader);
    void DrawLine(const glm::vec3& start, const glm::vec3& end, const Color& color, float width = 1.0f);
    void DrawPoint(const glm::vec3& position, const Color& color, float size = 1.0f);
    void DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const Color& color);
    void DrawRectangle(const glm::vec2& position, const glm::vec2& size, const Color& color);
    void DrawCircle(const glm::vec2& center, float radius, const Color& color, uint32_t segments = 32);
    
    // Immediate mode rendering (for debug/UI)
    void BeginImmediate();
    void EndImmediate();
    void ImmediateVertex(const glm::vec3& position, const Color& color = Color::White());
    void ImmediateTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, 
                          const Color& color = Color::White());
    
    // Enhanced shader management via ShaderManager
    std::shared_ptr<Shader> CreateShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
    std::shared_ptr<Shader> CreateShaderFromFiles(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    std::shared_ptr<Shader> CreateBuiltInShader(const std::string& name, const std::string& shaderType);
    std::shared_ptr<Shader> GetShader(const std::string& name);
    bool HasShader(const std::string& name) const;
    void RemoveShader(const std::string& name);
    void UseShader(const std::string& name);
    void UseShader(std::shared_ptr<Shader> shader);
    
    // Legacy shader management for backward compatibility
    uint32_t CreateShader(const std::string& vertexSource, const std::string& fragmentSource);
    void DeleteShader(uint32_t shader);
    void UseShader(uint32_t shader);
    
    uint32_t CreateTexture(uint32_t width, uint32_t height, const void* data = nullptr);
    void DeleteTexture(uint32_t texture);
    void BindTexture(uint32_t texture, uint32_t unit = 0);
    
    uint32_t CreateMesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices);
    void DeleteMesh(uint32_t mesh);
    void DrawMesh(uint32_t mesh, uint32_t shader);
    
    // Utility methods
    bool IsInitialized() const { return m_initialized; }
    IGraphicsAPI* GetGraphicsAPI() { return m_graphicsAPI.get(); }
    const RenderState& GetRenderState() const { return m_renderState; }
    ShaderManager* GetShaderManager() { return &m_shaderManager; }
    
    // Error handling
    bool CheckError();
    std::string GetErrorString();
    
private:
    // Core components
    std::unique_ptr<IGraphicsAPI> m_graphicsAPI;
    ShaderManager m_shaderManager;
    bool m_initialized;
    
    // Render state
    RenderState m_renderState;
    
    // Command queue
    std::vector<RenderCommand> m_commandQueue;
    bool m_recordingCommands;
    
    // Immediate mode state
    struct ImmediateState {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        uint32_t vao, vbo, ebo;
        std::shared_ptr<Shader> shader;
        bool active;
    } m_immediateState;
    
    // Resource management (legacy for backward compatibility)
    std::unordered_map<uint32_t, std::unique_ptr<Shader>> m_shaders;
    std::unordered_map<uint32_t, std::unique_ptr<Texture>> m_textures;
    std::unordered_map<uint32_t, std::unique_ptr<Mesh>> m_meshes;
    
    // Current active shader for rendering
    std::shared_ptr<Shader> m_currentShader;
    
    // Helper methods
    void ExecuteCommand(const RenderCommand& command);
    void FlushCommandQueue();
    void SetupImmediateMode();
    void CleanupImmediateMode();
    uint32_t GenerateResourceID();
    
    // Resource ID counter
    uint32_t m_nextResourceID;
};

} // namespace Tempest 