#pragma once

#include <string>
#include <cstdint>

namespace Tempest {

class Texture {
public:
    Texture();
    ~Texture();
    
    // Texture creation
    bool LoadFromFile(const std::string& filePath);
    bool Create(uint32_t width, uint32_t height, const void* data = nullptr);
    
    // Getters
    uint32_t GetWidth() const { return m_width; }
    uint32_t GetHeight() const { return m_height; }
    uint32_t GetID() const { return m_id; }
    uint32_t GetTextureID() const { return m_id; }
    bool IsValid() const { return m_id != 0; }
    
    // Setters
    void SetTextureID(uint32_t id) { m_id = id; }
    void SetDimensions(uint32_t width, uint32_t height) { m_width = width; m_height = height; }
    
private:
    uint32_t m_id;
    uint32_t m_width;
    uint32_t m_height;
};

} // namespace Tempest 