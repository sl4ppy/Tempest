#include "ShaderManager.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace Tempest {

// Built-in shader definitions
const std::unordered_map<std::string, std::pair<std::string, std::string>> ShaderManager::s_builtInShaders = {
    {"basic", {
        // Basic vertex shader
        R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 Color;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Color = aColor;
}
        )",
        // Basic fragment shader
        R"(
#version 330 core
in vec4 Color;
out vec4 FragColor;

void main() {
    FragColor = Color;
}
        )"
    }},
    {"textured", {
        // Textured vertex shader
        R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
        )",
        // Textured fragment shader
        R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform vec4 tintColor;

void main() {
    FragColor = texture(texture1, TexCoord) * tintColor;
}
        )"
    }},
    {"line", {
        // Line vertex shader
        R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;

out vec4 Color;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Color = color;
}
        )",
        // Line fragment shader
        R"(
#version 330 core
in vec4 Color;
out vec4 FragColor;

void main() {
    FragColor = Color;
}
        )"
    }}
};

ShaderManager::ShaderManager()
    : m_graphicsAPI(nullptr)
    , m_initialized(false) {
}

ShaderManager::~ShaderManager() {
    Shutdown();
}

bool ShaderManager::Initialize(IGraphicsAPI* graphicsAPI) {
    if (!graphicsAPI) {
        spdlog::error("ShaderManager: Invalid graphics API pointer");
        return false;
    }
    
    m_graphicsAPI = graphicsAPI;
    m_initialized = true;
    
    // Create built-in shaders
    CreateBuiltInShaders();
    
    spdlog::info("ShaderManager initialized successfully");
    return true;
}

void ShaderManager::Shutdown() {
    if (!m_initialized) return;
    
    ClearAllShaders();
    
    m_graphicsAPI = nullptr;
    m_initialized = false;
    
    spdlog::info("ShaderManager shutdown complete");
}

std::shared_ptr<Shader> ShaderManager::CreateShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource) {
    if (!m_initialized) {
        spdlog::error("ShaderManager not initialized");
        return nullptr;
    }
    
    // Check if shader already exists
    if (HasShader(name)) {
        spdlog::warn("Shader '{}' already exists, returning existing shader", name);
        return GetShader(name);
    }
    
    // Create new shader
    if (CompileAndCacheShader(name, vertexSource, fragmentSource)) {
        CacheShaderSource(name, vertexSource, fragmentSource);
        spdlog::info("Shader '{}' created successfully", name);
        return GetShader(name);
    }
    
    spdlog::error("Failed to create shader '{}'", name);
    return nullptr;
}

std::shared_ptr<Shader> ShaderManager::CreateShaderFromFiles(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
    if (!m_initialized) {
        spdlog::error("ShaderManager not initialized");
        return nullptr;
    }
    
    // Check if shader already exists
    if (HasShader(name)) {
        spdlog::warn("Shader '{}' already exists, returning existing shader", name);
        return GetShader(name);
    }
    
    // Load shader sources from files
    std::string vertexSource = LoadShaderFile(vertexPath);
    std::string fragmentSource = LoadShaderFile(fragmentPath);
    
    if (vertexSource.empty() || fragmentSource.empty()) {
        spdlog::error("Failed to load shader files for '{}'", name);
        return nullptr;
    }
    
    // Create shader
    if (CompileAndCacheShader(name, vertexSource, fragmentSource)) {
        CacheShaderSource(name, vertexSource, fragmentSource, true, vertexPath, fragmentPath);
        spdlog::info("Shader '{}' created from files successfully", name);
        return GetShader(name);
    }
    
    spdlog::error("Failed to create shader '{}' from files", name);
    return nullptr;
}

std::shared_ptr<Shader> ShaderManager::CreateBuiltInShader(const std::string& name, const std::string& shaderType) {
    if (!m_initialized) {
        spdlog::error("ShaderManager not initialized");
        return nullptr;
    }
    
    // Check if shader already exists
    if (HasShader(name)) {
        spdlog::warn("Shader '{}' already exists, returning existing shader", name);
        return GetShader(name);
    }
    
    // Get built-in shader source
    auto sources = GetBuiltInShaderSource(shaderType);
    if (sources.first.empty() || sources.second.empty()) {
        spdlog::error("Built-in shader type '{}' not found", shaderType);
        return nullptr;
    }
    
    // Create shader
    if (CompileAndCacheShader(name, sources.first, sources.second)) {
        CacheShaderSource(name, sources.first, sources.second, false, "", "", true, shaderType);
        spdlog::info("Built-in shader '{}' (type: {}) created successfully", name, shaderType);
        return GetShader(name);
    }
    
    spdlog::error("Failed to create built-in shader '{}' (type: {})", name, shaderType);
    return nullptr;
}

std::shared_ptr<Shader> ShaderManager::GetShader(const std::string& name) {
    auto it = m_shaders.find(name);
    if (it != m_shaders.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Shader> ShaderManager::GetShader(const std::string& name) const {
    auto it = m_shaders.find(name);
    if (it != m_shaders.end()) {
        return it->second;
    }
    return nullptr;
}

bool ShaderManager::HasShader(const std::string& name) const {
    return m_shaders.find(name) != m_shaders.end();
}

void ShaderManager::RemoveShader(const std::string& name) {
    auto it = m_shaders.find(name);
    if (it != m_shaders.end()) {
        m_shaders.erase(it);
        m_shaderSources.erase(name);
        spdlog::info("Shader '{}' removed", name);
    }
}

void ShaderManager::ClearAllShaders() {
    m_shaders.clear();
    m_shaderSources.clear();
    spdlog::info("All shaders cleared");
}

void ShaderManager::CreateBuiltInShaders() {
    // Create basic shaders
    CreateBuiltInShader("basic", "basic");
    CreateBuiltInShader("textured", "textured");
    CreateBuiltInShader("line", "line");
    
    spdlog::info("Built-in shaders created");
}

std::vector<std::string> ShaderManager::GetShaderNames() const {
    std::vector<std::string> names;
    names.reserve(m_shaders.size());
    
    for (const auto& pair : m_shaders) {
        names.push_back(pair.first);
    }
    
    return names;
}

void ShaderManager::ReloadShader(const std::string& name) {
    auto sourceIt = m_shaderSources.find(name);
    if (sourceIt == m_shaderSources.end()) {
        spdlog::error("Cannot reload shader '{}' - source info not found", name);
        return;
    }
    
    const ShaderSourceInfo& info = sourceIt->second;
    
    // Remove existing shader
    RemoveShader(name);
    
    // Recreate shader based on source info
    if (info.isBuiltIn) {
        CreateBuiltInShader(name, info.builtInType);
    } else if (info.isFromFile) {
        CreateShaderFromFiles(name, info.vertexPath, info.fragmentPath);
    } else {
        CreateShader(name, info.vertexSource, info.fragmentSource);
    }
    
    spdlog::info("Shader '{}' reloaded", name);
}

void ShaderManager::ReloadAllShaders() {
    std::vector<std::string> names = GetShaderNames();
    for (const std::string& name : names) {
        ReloadShader(name);
    }
    spdlog::info("All shaders reloaded");
}

bool ShaderManager::ValidateShader(const std::string& name) const {
    auto shader = GetShader(name);
    if (!shader) {
        return false;
    }
    
    return shader->CheckCompilationErrors();
}

bool ShaderManager::ValidateAllShaders() const {
    for (const auto& pair : m_shaders) {
        if (!ValidateShader(pair.first)) {
            return false;
        }
    }
    return true;
}

// Private helper methods
std::string ShaderManager::LoadShaderFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        spdlog::error("Failed to open shader file: {}", filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::pair<std::string, std::string> ShaderManager::GetBuiltInShaderSource(const std::string& shaderType) {
    auto it = s_builtInShaders.find(shaderType);
    if (it != s_builtInShaders.end()) {
        return it->second;
    }
    return {"", ""};
}

bool ShaderManager::CompileAndCacheShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource) {
    auto shader = std::make_shared<Shader>();
    
    if (!shader->Create(vertexSource, fragmentSource, m_graphicsAPI)) {
        spdlog::error("Failed to compile shader '{}'", name);
        return false;
    }
    
    m_shaders[name] = shader;
    return true;
}

void ShaderManager::CacheShaderSource(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource, bool isFromFile, const std::string& vertexPath, const std::string& fragmentPath, bool isBuiltIn, const std::string& builtInType) {
    ShaderSourceInfo info;
    info.vertexSource = vertexSource;
    info.fragmentSource = fragmentSource;
    info.vertexPath = vertexPath;
    info.fragmentPath = fragmentPath;
    info.isFromFile = isFromFile;
    info.isBuiltIn = isBuiltIn;
    info.builtInType = builtInType;
    
    m_shaderSources[name] = info;
}

} // namespace Tempest 