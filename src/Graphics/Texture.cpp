#include "Texture.h"
#include <spdlog/spdlog.h>

namespace Tempest {

Texture::Texture() : m_id(0), m_width(0), m_height(0) {
}

Texture::~Texture() {
    // TODO: Implement texture cleanup when real OpenGL is integrated
}

bool Texture::LoadFromFile(const std::string& filePath) {
    // TODO: Implement texture loading from file
    spdlog::warn("Texture::LoadFromFile not yet implemented");
    return false;
}

bool Texture::Create(uint32_t width, uint32_t height, const void* data) {
    // TODO: Implement texture creation
    spdlog::warn("Texture::Create not yet implemented");
    m_width = width;
    m_height = height;
    return false;
}

} // namespace Tempest 