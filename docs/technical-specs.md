# Technical Specifications

## System Requirements

### Minimum Requirements
- **OS**: Windows 10, macOS 10.15, Ubuntu 20.04
- **CPU**: Intel Core i3-6100 / AMD FX-6300
- **RAM**: 4 GB
- **GPU**: OpenGL 4.3 compatible graphics card
- **Storage**: 100 MB available space
- **Input**: Keyboard and mouse

### Recommended Requirements
- **OS**: Windows 11, macOS 12, Ubuntu 22.04
- **CPU**: Intel Core i5-8400 / AMD Ryzen 5 2600
- **RAM**: 8 GB
- **GPU**: OpenGL 4.6 / Vulkan 1.1 compatible graphics card
- **Storage**: 500 MB available space
- **Input**: Gamepad/controller support

### Development Requirements
- **Compiler**: GCC 10+, Clang 12+, MSVC 2019+
- **CMake**: 3.20+
- **Git**: 2.30+
- **IDE**: Visual Studio Code, CLion, or similar

## Core Engine Specifications

### Memory Management
```cpp
// Memory allocation strategy based on original game layout
class MemoryManager {
    static constexpr size_t GAME_STATE_SIZE = 1024 * 1024;    // 1MB
    static constexpr size_t ENTITY_DATA_SIZE = 2 * 1024 * 1024; // 2MB
    static constexpr size_t GRAPHICS_DATA_SIZE = 4 * 1024 * 1024; // 4MB
    static constexpr size_t AUDIO_DATA_SIZE = 1024 * 1024;    // 1MB
    static constexpr size_t SYSTEM_DATA_SIZE = 1024 * 1024;   // 1MB
};
```

### Critical Memory Mappings (Based on ASM Analysis)

#### Game State Variables (RAM 0000-009F)
```cpp
struct GameState {
    uint8_t gamestate;        // RAM 0000: Game state (0x00=startup, 0x04=playing, etc.)
    uint8_t timectr;          // RAM 0004: Time-based counter
    uint8_t player_control;   // RAM 0005: Player input mode (0x00=attract, 0xC0=gameplay)
    uint8_t credits;          // RAM 0006: Number of game credits (max 0x28)
    uint8_t zap_fire_shadow;  // RAM 0008: Shadow register for controls
    uint8_t curplayer;        // RAM 003D: Current player (0=player 1, 1=player 2)
    uint8_t p1_score_l;       // RAM 0040: Player 1 score (lowest 2 digits)
    uint8_t p1_score_m;       // RAM 0041: Player 1 score (middle 2 digits)
    uint8_t p1_score_h;       // RAM 0042: Player 1 score (highest 2 digits)
    uint8_t p1_level;         // RAM 0046: Level for Player 1
    uint8_t p1_lives;         // RAM 0048: Lives for Player 1
    uint8_t curlevel;         // RAM 009F: Current level - 1
};
```

#### Input System (RAM 004C-0050)
```cpp
struct InputState {
    uint8_t zap_fire_tmp1;    // RAM 004C: Button bitmap (current status)
    uint8_t zap_fire_debounce; // RAM 004D: Button bitmap with debounce
    uint8_t zap_fire_new;     // RAM 004E: Button bitmap (8=superzapper, 0x10=fire, 0x20=P1, 0x40=P2)
    uint8_t zap_fire_tmp2;    // RAM 004F: Same as 004D
    uint8_t spinner_change;   // RAM 0050: Spinner position change since last read
    uint8_t player_position;  // RAM 0051: Player's position in tunnel (0x00-0xFF)
};
```

#### Enemy System (RAM 0108-0146)
```cpp
struct EnemyState {
    uint8_t enemies_in;       // RAM 0108: Number of enemies INSIDE the tube
    uint8_t enemies_top;      // RAM 0109: Number of enemies at the TOP of the tube
    uint8_t min_flippers;     // RAM 0129: Proportion of Flippers to release
    uint8_t min_pulsars;      // RAM 012A: Proportion of Pulsars to release
    uint8_t min_tankers;      // RAM 012B: Proportion of Tankers to release
    uint8_t min_spikers;      // RAM 012C: Proportion of Spikers to release
    uint8_t min_fuzzballs;    // RAM 012D: Proportion of Fuseballs to release
    uint8_t max_flippers;     // RAM 012E: Max # of Flippers allowed in tunnel
    uint8_t max_pulsars;      // RAM 012F: Max # of Pulsars allowed in tunnel
    uint8_t max_tankers;      // RAM 0130: Max # of Tankers allowed in tunnel
    uint8_t max_spikers;      // RAM 0131: Max # of Spikers allowed in tunnel
    uint8_t max_fuzzballs;    // RAM 0132: Max # of Fuseballs allowed in tunnel
    uint8_t ply_shotcnt;      // RAM 0135: Number of shots (0-8) the player has active
    uint8_t cur_flippers;     // RAM 0142: Number of Flippers currently in tunnel
    uint8_t cur_pulsars;      // RAM 0143: Number of Pulsars currently in tunnel
    uint8_t cur_tankers;      // RAM 0144: Number of Tankers currently in tunnel
    uint8_t cur_spikers;      // RAM 0145: Number of Spikers currently in tunnel
    uint8_t cur_fuzzballs;    // RAM 0146: Number of Fuseballs currently in tunnel
};
```

#### Player System (RAM 0200-0202)
```cpp
struct PlayerState {
    uint8_t player_seg_minus1; // RAM 0200: Player's position in tunnel -1 (0-F only)
    uint8_t player_seg;        // RAM 0201: Player's position in tunnel (0-F, 0x80/0x81=dead)
    uint8_t player_along;      // RAM 0202: Y position of player (usually = 0x10)
};
```

#### Enemy Arrays (RAM 0243-0290)
```cpp
struct EnemyData {
    uint8_t enemy_timers[64];  // RAM 0243-0282: Countdown timers before enemies appear
    uint8_t enemy_movement[7]; // RAM 0283-0289: Movement and type for enemies 1-7
    uint8_t enemy_more_mov[7]; // RAM 028A-0290: More movement information for enemies 1-7
};
```

#### Bullet System (RAM 02AD-02DA)
```cpp
struct BulletState {
    uint8_t bullet_segments[8]; // RAM 02AD-02B4: Segments that player bullets are firing down -1
    uint8_t bullet_positions[8]; // RAM 02C0-02C7: Segments that player bullets are firing down
    uint8_t bullet_y_pos[8];    // RAM 02D3-02DA: Y positions of player bullets
};
```

### Game State Management
```cpp
// Game states based on original gamestate variable analysis
enum class GameState {
    Startup = 0x00,           // Entered briefly at game startup
    LevelStart = 0x02,        // Entered briefly at beginning of first level
    Playing = 0x04,           // Playing (including attract mode)
    PlayerDeath = 0x06,       // Entered briefly on player death
    LevelBegin = 0x08,        // Set briefly at beginning of each level
    HighScore = 0x0a,         // High-score, logo screen, AVOID SPIKES
    Unused = 0x0c,            // Unused (jump table holds 0x0000)
    LevelEnd = 0x0e,          // Entered briefly when zooming off end of level
    EnterInitials = 0x12,     // Entering initials
    LevelSelect = 0x16,       // Starting level selection screen
    ZoomingLevel = 0x18,      // Zooming new level in
    Unknown1 = 0x1a,          // Unknown state
    Unknown2 = 0x1c,          // Unknown state
    Unknown3 = 0x1e,          // Unknown state
    Descending = 0x20,        // Descending down tube at level end
    ServiceMode = 0x22,       // Non-selftest service mode display
    Explosion = 0x24          // High-score explosion
};
```

### Entity Component System (ECS)
```cpp
// Core ECS types based on original game data structures
using EntityID = uint32_t;
using ComponentTypeID = uint32_t;

struct Entity {
    EntityID id;
    std::bitset<64> componentMask;
};

// Component types based on original game data
struct Transform {
    float x, y, z;
    float scale;
    float rotation;
};

struct Enemy {
    enum class Type { 
        Flipper = 1, 
        Pulsar = 2, 
        Tanker = 3, 
        Spiker = 4, 
        Fuzzball = 5 
    };
    Type type;
    uint8_t segment;          // 0-F segment position
    float along;              // Position along tube depth
    uint8_t health;
    uint8_t aiState;
    uint8_t movementType;     // 0=no adjustment, 8=clockwise, 0xC=counter-clockwise
};

struct Player {
    uint8_t segment;          // Current segment (0-F)
    uint8_t lives;            // Remaining lives
    uint8_t superZapperUses;  // Number of super zapper uses (0-2)
    bool isDead;              // Player death state
};

struct Projectile {
    uint8_t segment;          // Target segment
    float yPosition;          // Y position in tube
    bool isPlayerShot;        // Player or enemy projectile
    uint8_t damage;
};
```

## Graphics Specifications

### Rendering Pipeline
```cpp
// Vector graphics rendering based on original vsdraw commands
struct VectorCommand {
    enum class Type {
        Move,       // Move to position
        Draw,       // Draw line to position
        Scale,      // Set scale
        Color,      // Set color
        Return      // End shape
    };
    
    Type type;
    float x, y, z;
    uint8_t color;
    float scale;
};

// Tube geometry specifications based on original game
struct TubeGeometry {
    static constexpr uint8_t SEGMENTS = 16;
    static constexpr float DEPTH_SCALE = 1.0f;
    static constexpr float PERSPECTIVE_FACTOR = 0.8f;
    
    std::array<Vector2f, SEGMENTS> segmentPositions;
    std::array<float, SEGMENTS> segmentScales;
};
```

### Shader Specifications
```glsl
// Vertex shader for vector graphics
#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in float aScale;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

out vec3 vColor;
out float vScale;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
    vColor = aColor;
    vScale = aScale;
}
```

```glsl
// Fragment shader for vector graphics
#version 460 core

in vec3 vColor;
in float vScale;

out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}
```

## Audio Specifications

### Audio System
```cpp
// Audio specifications based on POKEY chip recreation
struct AudioSpec {
    static constexpr uint32_t SAMPLE_RATE = 44100;
    static constexpr uint8_t CHANNELS = 4; // POKEY had 4 channels
    static constexpr uint16_t BUFFER_SIZE = 1024;
    static constexpr float MASTER_VOLUME = 0.7f;
};

// Sound effect specifications based on original game
enum class SoundEffect {
    ZapWeapon,        // Zap weapon sound
    FireWeapon,       // Fire weapon sound
    EnemyExplosion,   // Enemy destruction
    PlayerExplosion,  // Player death
    EnemyShot,        // Enemy projectile
    PlayerDeath,      // Player death sequence
    LevelComplete,    // Level completion
    HighScore,        // High score achievement
    SuperZapper,      // Super zapper activation
    SpinnerMove       // Spinner movement sound
};
```

### Audio Synthesis
```cpp
// POKEY chip sound synthesis recreation
class POKEYSynthesizer {
    struct Channel {
        float frequency;
        float volume;
        bool enabled;
        uint8_t waveform; // Square, sawtooth, triangle, noise
    };
    
    std::array<Channel, 4> channels;
    
public:
    void GenerateSamples(float* buffer, size_t count);
    void SetChannelFrequency(uint8_t channel, float freq);
    void SetChannelVolume(uint8_t channel, float volume);
    void SetChannelWaveform(uint8_t channel, uint8_t waveform);
};
```

## Performance Considerations

### Optimization Targets
- **60 FPS**: Consistent frame rate
- **<16ms**: Frame time budget
- **<100MB**: Memory usage
- **<5%**: CPU usage on modern hardware

### Memory Layout Optimization
```cpp
// Memory layout based on original game structure
struct GameMemoryLayout {
    // Game state: ~1MB
    GameState gameState;
    
    // Entity data: ~2MB
    std::array<Enemy, 64> enemies;
    std::array<Projectile, 16> projectiles;
    Player player;
    
    // Graphics data: ~4MB
    std::vector<VectorCommand> vectorCommands;
    std::vector<float> vertexData;
    
    // Audio data: ~1MB
    std::array<float, 44100> audioBuffer;
    
    // System data: ~1MB
    std::vector<Event> eventQueue;
    std::vector<Component> componentData;
};
```

### Critical Performance Paths
1. **Enemy AI Updates**: Must complete within 2ms per frame
2. **Collision Detection**: Spatial partitioning for tube segments
3. **Vector Rendering**: Command batching for efficient GPU usage
4. **Audio Synthesis**: Real-time POKEY chip emulation
5. **Input Processing**: Low-latency response for player controls 