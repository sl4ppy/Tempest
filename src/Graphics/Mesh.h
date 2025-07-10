#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Tempest {

class Mesh {
public:
    Mesh();
    ~Mesh();
    
    // Mesh creation
    void SetVertices(const std::vector<float>& vertices);
    void SetIndices(const std::vector<uint32_t>& indices);
    void SetNormals(const std::vector<float>& normals);
    void SetTexCoords(const std::vector<float>& texCoords);
    
    // Getters
    const std::vector<float>& GetVertices() const { return m_vertices; }
    const std::vector<uint32_t>& GetIndices() const { return m_indices; }
    const std::vector<float>& GetNormals() const { return m_normals; }
    const std::vector<float>& GetTexCoords() const { return m_texCoords; }
    
    // Utility
    uint32_t GetVertexCount() const { return m_vertexCount; }
    uint32_t GetIndexCount() const { return m_indexCount; }
    bool HasNormals() const { return !m_normals.empty(); }
    bool HasTexCoords() const { return !m_texCoords.empty(); }
    
    // OpenGL resource management
    void SetVAO(uint32_t vao) { m_vao = vao; }
    void SetVBO(uint32_t vbo) { m_vbo = vbo; }
    void SetEBO(uint32_t ebo) { m_ebo = ebo; }
    void SetVertexCount(uint32_t count) { m_vertexCount = count; }
    void SetIndexCount(uint32_t count) { m_indexCount = count; }
    
    uint32_t GetVAO() const { return m_vao; }
    uint32_t GetVBO() const { return m_vbo; }
    uint32_t GetEBO() const { return m_ebo; }
    
private:
    std::vector<float> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<float> m_normals;
    std::vector<float> m_texCoords;
    
    // OpenGL resources
    uint32_t m_vao = 0;
    uint32_t m_vbo = 0;
    uint32_t m_ebo = 0;
    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;
};

} // namespace Tempest 