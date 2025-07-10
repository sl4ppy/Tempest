#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include "GraphicsAPI.h"

namespace Tempest {

// Forward declarations
struct Color;

class Shader {
public:
    Shader();
    ~Shader();
    
    // Creation methods
    bool Create(const std::string& vertexSource, const std::string& fragmentSource, IGraphicsAPI* graphicsAPI);
    bool CreateFromFiles(const std::string& vertexPath, const std::string& fragmentPath, IGraphicsAPI* graphicsAPI);
    void Destroy();
    
    // Usage
    void Use() const;
    
    // Uniform setters
    void SetBool(const std::string& name, bool value);
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat3(const std::string& name, const glm::mat3& value);
    void SetMat4(const std::string& name, const glm::mat4& value);
    
    // Convenience methods for common uniforms
    void SetProjectionMatrix(const glm::mat4& projection);
    void SetViewMatrix(const glm::mat4& view);
    void SetModelMatrix(const glm::mat4& model);
    void SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
    void SetColor(const std::string& name, const Color& color);
    
    // Utility methods
    bool CheckCompilationErrors() const;
    std::string GetCompilationLog() const;
    
    // Getters
    uint32_t GetProgram() const { return m_program; }
    bool IsValid() const { return m_program != 0; }
    
    // Graphics API access
    void SetProgram(uint32_t program) { m_program = program; }
    
private:
    uint32_t m_program;
    uint32_t m_vertexShader;
    uint32_t m_fragmentShader;
    IGraphicsAPI* m_graphicsAPI;
    
    // Uniform location cache
    mutable std::unordered_map<std::string, int> m_uniformLocations;
    
    // Helper methods
    bool CompileShader(uint32_t type, const std::string& source, uint32_t& shader);
    bool LinkProgram();
    int GetUniformLocation(const std::string& name) const;
    std::string LoadShaderSource(const std::string& filePath);
};

// Built-in shader types
namespace ShaderType {
    // Basic color shader
    extern const std::string BasicVertex;
    extern const std::string BasicFragment;
    
    // Textured shader
    extern const std::string TexturedVertex;
    extern const std::string TexturedFragment;
    
    // Line shader
    extern const std::string LineVertex;
    extern const std::string LineFragment;
    
    // Point shader
    extern const std::string PointVertex;
    extern const std::string PointFragment;
    
    // Tube geometry shader (for vector graphics)
    extern const std::string TubeVertex;
    extern const std::string TubeFragment;
}

} // namespace Tempest 