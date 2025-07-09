#pragma once

namespace Tempest {

class TempestGame {
public:
    TempestGame();
    ~TempestGame();

    void initialize();
    void update(float deltaTime);
    void render();
    void shutdown();
};

} // namespace Tempest 