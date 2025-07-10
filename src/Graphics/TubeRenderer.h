#pragma once

#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"
#include "../Game/TubeGeometry.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace Tempest {

/**
 * @brief Tube Renderer for 3D tube geometry rendering
 * 
 * Handles the rendering of the 3D tube geometry with perspective projection,
 * depth-based scaling, and proper 3D transformations. Based on the original
 * Tempest game's tube rendering system.
 */
class TubeRenderer {
public:
    TubeRenderer();
    ~TubeRenderer();
    
    // Initialization
    bool Initialize(Renderer* renderer);
    void Shutdown();
    
    // Tube rendering
    void RenderTube(const TubeGeometry& geometry);
    void RenderTubeWireframe(const TubeGeometry& geometry);
    void RenderSegment(uint8_t segment, float depth, const glm::vec4& color);
    void RenderSegmentHighlight(uint8_t segment, float depth, const glm::vec4& color);
    
    // Camera and viewport
    void SetCameraPosition(const glm::vec3& position);
    void SetCameraTarget(const glm::vec3& target);
    void SetProjectionMatrix(const glm::mat4& projection);
    void SetViewMatrix(const glm::mat4& view);
    
    // Tube configuration
    void SetTubeRadius(float radius);
    void SetTubeDepth(float depth);
    void SetSegmentCount(uint8_t segments);
    void SetDepthLevels(uint8_t levels);
    
    // Rendering options
    void EnableWireframe(bool enable);
    void EnableDepthTest(bool enable);
    void EnableBlending(bool enable);
    void SetWireframeColor(const glm::vec4& color);
    void SetSegmentColors(const std::vector<glm::vec4>& colors);
    
    // Utility functions
    glm::vec3 GetSegmentPosition(uint8_t segment, float depth) const;
    glm::vec3 GetSegmentNormal(uint8_t segment) const;
    bool IsSegmentVisible(uint8_t segment, float depth) const;
    float GetDepthScale(float depth) const;
    
    // Getters
    bool IsInitialized() const { return m_initialized; }
    Renderer* GetRenderer() const { return m_renderer; }
    const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }
    const glm::mat4& GetViewMatrix() const { return m_viewMatrix; }

private:
    // Core components
    Renderer* m_renderer;
    bool m_initialized;
    
    // Shaders
    std::unique_ptr<Shader> m_tubeShader;
    std::unique_ptr<Shader> m_wireframeShader;
    std::unique_ptr<Shader> m_segmentShader;
    
    // Meshes
    std::unique_ptr<Mesh> m_tubeMesh;
    std::unique_ptr<Mesh> m_wireframeMesh;
    std::unique_ptr<Mesh> m_segmentMesh;
    
    // Matrices
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_modelMatrix;
    
    // Camera
    glm::vec3 m_cameraPosition;
    glm::vec3 m_cameraTarget;
    
    // Tube configuration
    float m_tubeRadius;
    float m_tubeDepth;
    uint8_t m_segmentCount;
    uint8_t m_depthLevels;
    
    // Rendering options
    bool m_wireframeEnabled;
    bool m_depthTestEnabled;
    bool m_blendingEnabled;
    glm::vec4 m_wireframeColor;
    std::vector<glm::vec4> m_segmentColors;
    
    // Helper functions
    void CreateShaders();
    void CreateMeshes();
    void UpdateMatrices();
    void RenderTubeGeometry(const TubeGeometry& geometry);
    void RenderWireframeGeometry(const TubeGeometry& geometry);
    void RenderSegmentGeometry(uint8_t segment, float depth, const glm::vec4& color);
    
    // Vertex generation
    void GenerateTubeVertices(const TubeGeometry& geometry);
    void GenerateWireframeVertices(const TubeGeometry& geometry);
    void GenerateSegmentVertices(uint8_t segment, float depth);
    
    // Shader uniforms
    void SetShaderUniforms(Shader& shader);
    void SetTubeUniforms(Shader& shader, const TubeGeometry& geometry);
    void SetWireframeUniforms(Shader& shader);
    void SetSegmentUniforms(Shader& shader, uint8_t segment, float depth, const glm::vec4& color);
};

} // namespace Tempest 