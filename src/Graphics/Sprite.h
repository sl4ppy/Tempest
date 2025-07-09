#pragma once

namespace Tempest {

class Sprite {
public:
    Sprite();
    ~Sprite();

    void update(float deltaTime);
    void render();
};

} // namespace Tempest 