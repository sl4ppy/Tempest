#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "Shader.h"
#include "GraphicsAPI.h"

namespace Tempest {

/**
 * @brief Shader Manager for centralized shader resource management
 * 
 * Manages shader compilation, loading, caching, and reuse throughout the application.
 * Supports loading from source strings, files, and built-in shader templates.
 */
class ShaderManager {
public:
    ShaderManager();
    ~ShaderManager();

    // Initialization
    bool Initialize(IGraphicsAPI* graphicsAPI);
    void Shutdown();

    // Shader creation and management
    std::shared_ptr<Shader> CreateShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
    std::shared_ptr<Shader> CreateShaderFromFiles(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    std::shared_ptr<Shader> CreateBuiltInShader(const std::string& name, const std::string& shaderType);
    
    // Shader retrieval
    std::shared_ptr<Shader> GetShader(const std::string& name);
    std::shared_ptr<Shader> GetShader(const std::string& name) const;
    bool HasShader(const std::string& name) const;
    
    // Shader management
    void RemoveShader(const std::string& name);
    void ClearAllShaders();
    
    // Built-in shaders
    void CreateBuiltInShaders();
    
    // Utility functions
    size_t GetShaderCount() const { return m_shaders.size(); }
    std::vector<std::string> GetShaderNames() const;
    
    // Resource management
    void ReloadShader(const std::string& name);
    void ReloadAllShaders();
    
    // Validation
    bool ValidateShader(const std::string& name) const;
    bool ValidateAllShaders() const;
    
    // Getters
    bool IsInitialized() const { return m_initialized; }
    IGraphicsAPI* GetGraphicsAPI() const { return m_graphicsAPI; }

private:
    // Core components
    IGraphicsAPI* m_graphicsAPI;
    bool m_initialized;
    
    // Shader cache
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
    
    // Shader source cache (for reloading)
    struct ShaderSourceInfo {
        std::string vertexSource;
        std::string fragmentSource;
        std::string vertexPath;
        std::string fragmentPath;
        bool isFromFile;
        bool isBuiltIn;
        std::string builtInType;
    };
    std::unordered_map<std::string, ShaderSourceInfo> m_shaderSources;
    
    // Built-in shader definitions
    static const std::unordered_map<std::string, std::pair<std::string, std::string>> s_builtInShaders;
    
    // Helper functions
    std::string LoadShaderFile(const std::string& filePath);
    std::pair<std::string, std::string> GetBuiltInShaderSource(const std::string& shaderType);
    bool CompileAndCacheShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
    void CacheShaderSource(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource, bool isFromFile = false, const std::string& vertexPath = "", const std::string& fragmentPath = "", bool isBuiltIn = false, const std::string& builtInType = "");
};

} // namespace Tempest 