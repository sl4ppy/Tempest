#include "Mesh.h"

namespace Tempest {

Mesh::Mesh() {
}

Mesh::~Mesh() {
}

void Mesh::SetVertices(const std::vector<float>& vertices) {
    m_vertices = vertices;
}

void Mesh::SetIndices(const std::vector<uint32_t>& indices) {
    m_indices = indices;
}

void Mesh::SetNormals(const std::vector<float>& normals) {
    m_normals = normals;
}

void Mesh::SetTexCoords(const std::vector<float>& texCoords) {
    m_texCoords = texCoords;
}

} // namespace Tempest 