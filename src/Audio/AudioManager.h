#pragma once

namespace Tempest {

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void initialize();
    void playSound();
    void shutdown();
};

} // namespace Tempest 