#pragma once

namespace Tempest {

class Keyboard {
public:
    Keyboard();
    ~Keyboard();

    void update();
    bool isKeyPressed();
};

} // namespace Tempest 