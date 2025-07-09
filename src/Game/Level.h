#pragma once

namespace Tempest {

class Level {
public:
    Level();
    ~Level();

    void update(float deltaTime);
    void render();
};

} // namespace Tempest 