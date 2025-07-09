#pragma once

namespace Tempest {

class InputManager {
public:
    InputManager();
    ~InputManager();

    void initialize();
    void update();
    void shutdown();
};

} // namespace Tempest 