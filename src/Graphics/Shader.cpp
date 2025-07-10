#include "Shader.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace Tempest {

Shader::Shader() : m_program(0), m_vertexShader(0), m_fragmentShader(0), m_graphicsAPI(nullptr) {
}

Shader::~Shader() {
    Destroy();
}

bool Shader::Create(const std::string& vertexSource, const std::string& fragmentSource, IGraphicsAPI* graphicsAPI) {
    if (!graphicsAPI) {
        spdlog::error("Graphics API is required for shader creation");
        return false;
    }
    
    m_graphicsAPI = graphicsAPI;
    
    // Compile vertex shader
    m_vertexShader = m_graphicsAPI->CreateShader(0x8B31, vertexSource); // GL_VERTEX_SHADER = 0x8B31
    if (m_vertexShader == 0) {
        spdlog::error("Failed to compile vertex shader");
        return false;
    }
    
    // Compile fragment shader  
    m_fragmentShader = m_graphicsAPI->CreateShader(0x8B30, fragmentSource); // GL_FRAGMENT_SHADER = 0x8B30
    if (m_fragmentShader == 0) {
        spdlog::error("Failed to compile fragment shader");
        m_graphicsAPI->DeleteShader(m_vertexShader);
        m_vertexShader = 0;
        return false;
    }
    
    // Create program and link shaders
    std::vector<uint32_t> shaders = {m_vertexShader, m_fragmentShader};
    m_program = m_graphicsAPI->CreateProgram(shaders);
    if (m_program == 0) {
        spdlog::error("Failed to create shader program");
        m_graphicsAPI->DeleteShader(m_vertexShader);
        m_graphicsAPI->DeleteShader(m_fragmentShader);
        m_vertexShader = 0;
        m_fragmentShader = 0;
        return false;
    }
    
    // Clean up individual shaders (they're now part of the program)
    m_graphicsAPI->DeleteShader(m_vertexShader);
    m_graphicsAPI->DeleteShader(m_fragmentShader);
    m_vertexShader = 0;
    m_fragmentShader = 0;
    
    spdlog::info("Shader created successfully (program ID: {})", m_program);
    return true;
}

bool Shader::CreateFromFiles(const std::string& vertexPath, const std::string& fragmentPath, IGraphicsAPI* graphicsAPI) {
    std::string vertexSource = LoadShaderSource(vertexPath);
    std::string fragmentSource = LoadShaderSource(fragmentPath);
    
    if (vertexSource.empty() || fragmentSource.empty()) {
        spdlog::error("Failed to load shader sources from files");
        return false;
    }
    
    return Create(vertexSource, fragmentSource, graphicsAPI);
}

void Shader::Destroy() {
    if (m_program != 0 && m_graphicsAPI != nullptr) {
        m_graphicsAPI->DeleteProgram(m_program);
        m_program = 0;
    }
    
    // Clean up any remaining individual shaders
    if (m_vertexShader != 0 && m_graphicsAPI != nullptr) {
        m_graphicsAPI->DeleteShader(m_vertexShader);
        m_vertexShader = 0;
    }
    
    if (m_fragmentShader != 0 && m_graphicsAPI != nullptr) {
        m_graphicsAPI->DeleteShader(m_fragmentShader);
        m_fragmentShader = 0;
    }
    
    m_uniformLocations.clear();
    m_graphicsAPI = nullptr;
}

void Shader::Use() const {
    if (m_program != 0 && m_graphicsAPI != nullptr) {
        m_graphicsAPI->UseProgram(m_program);
    }
}

void Shader::SetBool(const std::string& name, bool value) {
    SetInt(name, static_cast<int>(value));
}

void Shader::SetInt(const std::string& name, int value) {
    if (m_program != 0 && m_graphicsAPI != nullptr) {
        m_graphicsAPI->SetUniform(m_program, name, value);
    }
}

void Shader::SetFloat(const std::string& name, float value) {
    if (m_program != 0 && m_graphicsAPI != nullptr) {
        m_graphicsAPI->SetUniform(m_program, name, value);
    }
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) {
    if (m_program != 0 && m_graphicsAPI != nullptr) {
        m_graphicsAPI->SetUniform(m_program, name, value);
    }
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
    if (m_program != 0 && m_graphicsAPI != nullptr) {
        m_graphicsAPI->SetUniform(m_program, name, value);
    }
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) {
    if (m_program != 0 && m_graphicsAPI != nullptr) {
        m_graphicsAPI->SetUniform(m_program, name, value);
    }
}

void Shader::SetMat3(const std::string& name, const glm::mat3& value) {
    if (m_program != 0 && m_graphicsAPI != nullptr) {
        m_graphicsAPI->SetUniform(m_program, name, value);
    }
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) {
    if (m_program != 0 && m_graphicsAPI != nullptr) {
        m_graphicsAPI->SetUniform(m_program, name, value);
    }
}

void Shader::SetProjectionMatrix(const glm::mat4& projection) {
    SetMat4("projection", projection);
}

void Shader::SetViewMatrix(const glm::mat4& view) {
    SetMat4("view", view);
}

void Shader::SetModelMatrix(const glm::mat4& model) {
    SetMat4("model", model);
}

void Shader::SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    SetModelMatrix(model);
    SetViewMatrix(view);
    SetProjectionMatrix(projection);
}

void Shader::SetColor(const std::string& name, const Color& color) {
    SetVec4(name, glm::vec4(color.r, color.g, color.b, color.a));
}

bool Shader::CheckCompilationErrors() const {
    return m_program != 0;
}

std::string Shader::GetCompilationLog() const {
    if (m_graphicsAPI != nullptr) {
        return m_graphicsAPI->GetErrorString();
    }
    return "No graphics API available";
}

// Private helper methods
bool Shader::CompileShader(uint32_t type, const std::string& source, uint32_t& shader) {
    // This method is now unused since we compile directly in Create()
    return true;
}

bool Shader::LinkProgram() {
    // This method is now unused since we link directly in Create()
    return true;
}

int Shader::GetUniformLocation(const std::string& name) const {
    // Check cache first
    auto it = m_uniformLocations.find(name);
    if (it != m_uniformLocations.end()) {
        return it->second;
    }
    
    // For now, return success since OpenGL API handles uniform location internally
    int location = 0;
    m_uniformLocations[name] = location;
    return location;
}

std::string Shader::LoadShaderSource(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        spdlog::error("Failed to open shader file: {}", filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Built-in shader sources
namespace ShaderType {
    const std::string BasicVertex = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Color = aColor;
}
)";

    const std::string BasicFragment = R"(
#version 460 core
in vec4 Color;
out vec4 FragColor;

void main() {
    FragColor = Color;
}
)";

    const std::string TexturedVertex = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

    const std::string TexturedFragment = R"(
#version 460 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoord);
}
)";

    const std::string LineVertex = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Color = aColor;
}
)";

    const std::string LineFragment = R"(
#version 460 core
in vec4 Color;
out vec4 FragColor;

void main() {
    FragColor = Color;
}
)";

    const std::string PointVertex = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float pointSize;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    gl_PointSize = pointSize;
    Color = aColor;
}
)";

    const std::string PointFragment = R"(
#version 460 core
in vec4 Color;
out vec4 FragColor;

void main() {
    FragColor = Color;
}
)";

    const std::string TubeVertex = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;

out vec4 Color;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
    Color = aColor;
}
)";

    const std::string TubeFragment = R"(
#version 460 core
in vec4 Color;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main() {
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * Color.rgb;
    FragColor = vec4(result, Color.a);
}
)";
}

} // namespace Tempest 