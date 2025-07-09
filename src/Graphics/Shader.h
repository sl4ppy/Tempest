#pragma once

namespace Tempest {

class Shader {
public:
    Shader();
    ~Shader();

    void compile();
    void use();
};

} // namespace Tempest 