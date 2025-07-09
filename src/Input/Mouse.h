#pragma once

namespace Tempest {

class Mouse {
public:
    Mouse();
    ~Mouse();

    void update();
    bool isButtonPressed();
};

} // namespace Tempest 