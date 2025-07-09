#pragma once

namespace Tempest {

class Renderer {
public:
    Renderer();
    ~Renderer();

    void initialize();
    void render();
    void shutdown();
};

} // namespace Tempest 