#pragma once

namespace Tempest {

class Texture {
public:
    Texture();
    ~Texture();

    void load();
    void bind();
};

} // namespace Tempest 