#pragma once

namespace Tempest {

class PlatformManager {
public:
    PlatformManager();
    ~PlatformManager();

    void initialize();
    void shutdown();
};

} // namespace Tempest 