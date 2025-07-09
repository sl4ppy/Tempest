# API Reference

## Core Engine API

### GameEngine

The main game engine class that coordinates all systems.

```cpp
namespace Tempest {

class GameEngine {
public:
    GameEngine();
    ~GameEngine();
    
    // Lifecycle
    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();
    bool IsRunning() const;
    
    // System access
    Renderer* GetRenderer();
    AudioManager* GetAudioManager();
    InputManager* GetInputManager();
    EntityManager* GetEntityManager();
    
    // Configuration
    void SetConfig(const GameConfig& config);
    const GameConfig& GetConfig() const;
    
private:
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<AudioManager> m_audioManager;
    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<EntityManager> m_entityManager;
    GameConfig m_config;
    bool m_isRunning;
};

} // namespace Tempest
```

### Entity Component System (ECS)

#### EntityManager

Manages entities and their components.

```cpp
class EntityManager {
public:
    // Entity creation/destruction
    EntityID CreateEntity();
    void DestroyEntity(EntityID entity);
    bool IsValid(EntityID entity) const;
    
    // Component management
    template<typename T>
    T* AddComponent(EntityID entity);
    
    template<typename T>
    T* GetComponent(EntityID entity);
    
    template<typename T>
    bool HasComponent(EntityID entity) const;
    
    template<typename T>
    void RemoveComponent(EntityID entity);
    
    // Querying
    template<typename... Components>
    std::vector<EntityID> GetEntitiesWith();
    
    // Iteration
    template<typename... Components>
    void ForEach(std::function<void(EntityID, Components*...)> func);
    
private:
    std::vector<Entity> m_entities;
    std::unordered_map<ComponentTypeID, std::vector<Component*>> m_components;
    std::queue<EntityID> m_freeEntities;
};
```

#### Component Types

```cpp
// Base component class
class Component {
public:
    virtual ~Component() = default;
    virtual void Reset() = 0;
};

// Transform component
struct Transform : public Component {
    Vector3f position = {0, 0, 0};
    Vector3f rotation = {0, 0, 0};
    Vector3f scale = {1, 1, 1};
    
    void Reset() override {
        position = {0, 0, 0};
        rotation = {0, 0, 0};
        scale = {1, 1, 1};
    }
};

// Player component
struct Player : public Component {
    uint8_t segment = 0;
    float along = 0.0f;  // Position along tube
    uint8_t lives = 3;
    uint32_t score = 0;
    bool isAlive = true;
    
    void Reset() override {
        segment = 0;
        along = 0.0f;
        lives = 3;
        score = 0;
        isAlive = true;
    }
};

// Enemy component
struct Enemy : public Component {
    enum class Type { Flipper, Pulsar, Tanker, Spiker, Fuzzball };
    
    Type type = Type::Flipper;
    uint8_t segment = 0;
    float along = 0.0f;
    uint8_t health = 1;
    uint8_t aiState = 0;
    float moveSpeed = 1.0f;
    
    void Reset() override {
        type = Type::Flipper;
        segment = 0;
        along = 0.0f;
        health = 1;
        aiState = 0;
        moveSpeed = 1.0f;
    }
};
```

## Graphics API

### Renderer

Main rendering system interface.

```cpp
class Renderer {
public:
    // Initialization
    bool Initialize(const RenderConfig& config);
    void Shutdown();
    
    // Frame management
    void BeginFrame();
    void EndFrame();
    void Present();
    
    // Drawing commands
    void DrawVector(const std::vector<VectorCommand>& commands);
    void DrawLine(const Vector3f& start, const Vector3f& end, const Color& color);
    void DrawText(const Vector3f& position, const std::string& text, const Color& color);
    void DrawParticles(const std::vector<Particle>& particles);
    
    // State management
    void SetViewMatrix(const Matrix4f& view);
    void SetProjectionMatrix(const Matrix4f& projection);
    void SetModelMatrix(const Matrix4f& model);
    
    // Utility
    void Clear(const Color& color);
    void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    
private:
    std::unique_ptr<IGraphicsAPI> m_graphicsAPI;
    std::unique_ptr<VectorRenderer> m_vectorRenderer;
    std::unique_ptr<ParticleSystem> m_particleSystem;
};
```

### VectorRenderer

Handles vector graphics rendering.

```cpp
class VectorRenderer {
public:
    // Vector command execution
    void ExecuteCommands(const std::vector<VectorCommand>& commands);
    
    // Primitive drawing
    void DrawVector(const Vector3f& start, const Vector3f& end, const Color& color);
    void DrawShape(const std::vector<Vector3f>& points, const Color& color);
    
    // Character rendering
    void DrawCharacter(char c, const Vector3f& position, float scale, const Color& color);
    void DrawString(const std::string& text, const Vector3f& position, float scale, const Color& color);
    
    // Tube geometry
    void SetTubeGeometry(const TubeGeometry& geometry);
    Vector3f TransformToTubeSpace(const Vector3f& worldPos) const;
    
private:
    TubeGeometry m_tubeGeometry;
    std::unique_ptr<Shader> m_vectorShader;
};
```

### ParticleSystem

Manages particle effects and explosions.

```cpp
class ParticleSystem {
public:
    // Particle management
    ParticleID CreateParticle(const ParticleConfig& config);
    void DestroyParticle(ParticleID id);
    
    // Effect creation
    void CreateExplosion(const Vector3f& position, const ExplosionConfig& config);
    void CreateTrail(const Vector3f& position, const TrailConfig& config);
    
    // Update and rendering
    void Update(float deltaTime);
    void Render();
    
    // Configuration
    void SetMaxParticles(uint32_t maxParticles);
    uint32_t GetActiveParticleCount() const;
    
private:
    std::vector<Particle> m_particles;
    std::queue<ParticleID> m_freeParticles;
    uint32_t m_maxParticles;
};
```

## Audio API

### AudioManager

Main audio system interface.

```cpp
class AudioManager {
public:
    // Initialization
    bool Initialize(const AudioConfig& config);
    void Shutdown();
    
    // Sound effects
    SoundID LoadSound(const std::string& path);
    void PlaySound(SoundID sound, float volume = 1.0f);
    void StopSound(SoundID sound);
    void SetSoundVolume(SoundID sound, float volume);
    
    // Music
    MusicID LoadMusic(const std::string& path);
    void PlayMusic(MusicID music, bool loop = true);
    void StopMusic();
    void PauseMusic();
    void ResumeMusic();
    
    // Volume control
    void SetMasterVolume(float volume);
    void SetSFXVolume(float volume);
    void SetMusicVolume(float volume);
    
    // POKEY synthesis
    void PlayPOKEYSound(uint8_t channel, float frequency, float volume, uint8_t waveform);
    void StopPOKEYSound(uint8_t channel);
    
private:
    std::unique_ptr<IAudioAPI> m_audioAPI;
    std::unique_ptr<POKEYSynthesizer> m_pokeySynthesizer;
    std::unordered_map<SoundID, std::unique_ptr<Sound>> m_sounds;
    std::unordered_map<MusicID, std::unique_ptr<Music>> m_music;
};
```

### POKEYSynthesizer

Recreates the original POKEY chip sound synthesis.

```cpp
class POKEYSynthesizer {
public:
    // Channel control
    void SetChannel(uint8_t channel, float frequency, float volume, uint8_t waveform);
    void StopChannel(uint8_t channel);
    
    // Waveform types
    enum class Waveform {
        Square = 0,
        Sawtooth = 1,
        Triangle = 2,
        Noise = 3
    };
    
    // Audio generation
    void GenerateSamples(float* buffer, size_t count);
    
    // Configuration
    void SetSampleRate(uint32_t sampleRate);
    void SetPolyphony(uint8_t polyphony);
    
private:
    struct Channel {
        float frequency = 0.0f;
        float volume = 0.0f;
        bool enabled = false;
        Waveform waveform = Waveform::Square;
        float phase = 0.0f;
    };
    
    std::array<Channel, 4> m_channels;
    uint32_t m_sampleRate = 44100;
};
```

## Input API

### InputManager

Main input system interface.

```cpp
class InputManager {
public:
    // Initialization
    bool Initialize(const InputConfig& config);
    void Shutdown();
    
    // Event processing
    void PollEvents();
    void ProcessEvents();
    
    // Input state
    bool IsKeyPressed(KeyCode key) const;
    bool IsKeyJustPressed(KeyCode key) const;
    bool IsKeyReleased(KeyCode key) const;
    
    bool IsMouseButtonPressed(MouseButton button) const;
    bool IsMouseButtonJustPressed(MouseButton button) const;
    Vector2f GetMousePosition() const;
    Vector2f GetMouseDelta() const;
    
    bool IsGamepadConnected(uint8_t gamepad) const;
    bool IsGamepadButtonPressed(uint8_t gamepad, GamepadButton button) const;
    float GetGamepadAxis(uint8_t gamepad, GamepadAxis axis) const;
    
    // Input mapping
    void SetKeyMapping(const std::string& action, KeyCode key);
    void SetGamepadMapping(const std::string& action, GamepadButton button);
    bool IsActionPressed(const std::string& action) const;
    
    // Callbacks
    using KeyCallback = std::function<void(KeyCode, bool)>;
    using MouseCallback = std::function<void(MouseButton, bool)>;
    using GamepadCallback = std::function<void(uint8_t, GamepadButton, bool)>;
    
    void SetKeyCallback(KeyCallback callback);
    void SetMouseCallback(MouseCallback callback);
    void SetGamepadCallback(GamepadCallback callback);
    
private:
    std::unique_ptr<IInputAPI> m_inputAPI;
    std::unordered_map<std::string, KeyCode> m_keyMappings;
    std::unordered_map<std::string, GamepadButton> m_gamepadMappings;
    InputState m_currentState;
    InputState m_previousState;
};
```

### Input Types

```cpp
// Key codes
enum class KeyCode {
    Unknown = 0,
    Space = 32,
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    LeftControl = 341,
    LeftShift = 340,
    // ... more keys
};

// Mouse buttons
enum class MouseButton {
    Left = 0,
    Right = 1,
    Middle = 2
};

// Gamepad buttons
enum class GamepadButton {
    A = 0,
    B = 1,
    X = 2,
    Y = 3,
    LeftBumper = 4,
    RightBumper = 5,
    Back = 6,
    Start = 7,
    LeftStick = 8,
    RightStick = 9
};

// Gamepad axes
enum class GamepadAxis {
    LeftX = 0,
    LeftY = 1,
    RightX = 2,
    RightY = 3,
    LeftTrigger = 4,
    RightTrigger = 5
};
```

## Game Logic API

### GameStateManager

Manages game states and transitions.

```cpp
class GameStateManager {
public:
    // State management
    void ChangeState(GameState newState);
    GameState GetCurrentState() const;
    bool IsInState(GameState state) const;
    
    // State transitions
    void PushState(GameState state);
    void PopState();
    void ClearStates();
    
    // State callbacks
    using StateCallback = std::function<void()>;
    void SetStateCallback(GameState state, StateCallback onEnter, StateCallback onExit);
    
    // Update
    void Update(float deltaTime);
    
private:
    std::stack<GameState> m_stateStack;
    std::unordered_map<GameState, std::pair<StateCallback, StateCallback>> m_callbacks;
};
```

### LevelManager

Manages level progression and difficulty.

```cpp
class LevelManager {
public:
    // Level management
    void LoadLevel(uint8_t levelNumber);
    void NextLevel();
    void RestartLevel();
    
    // Level information
    uint8_t GetCurrentLevel() const;
    uint8_t GetMaxLevel() const;
    bool IsLevelComplete() const;
    
    // Difficulty
    void SetDifficulty(Difficulty difficulty);
    Difficulty GetDifficulty() const;
    float GetDifficultyMultiplier() const;
    
    // Enemy spawning
    void SpawnEnemyWave(const EnemyWave& wave);
    void SpawnEnemy(Enemy::Type type, uint8_t segment, float along);
    
    // Level data
    const LevelData& GetCurrentLevelData() const;
    void SetLevelData(uint8_t level, const LevelData& data);
    
private:
    uint8_t m_currentLevel = 0;
    Difficulty m_difficulty = Difficulty::Normal;
    LevelData m_currentLevelData;
    std::vector<EnemyWave> m_pendingWaves;
};
```

### AISystem

Manages enemy AI behavior.

```cpp
class AISystem {
public:
    // AI updates
    void UpdateEnemyAI(EntityID enemy, float deltaTime);
    void UpdateAllEnemies(float deltaTime);
    
    // AI behavior
    void SetEnemyBehavior(EntityID enemy, const AIBehavior& behavior);
    AIBehavior GetEnemyBehavior(EntityID enemy) const;
    
    // AI decision making
    AIDecision CalculateDecision(EntityID enemy, EntityID player) const;
    Vector3f CalculateTargetPosition(EntityID enemy, EntityID player) const;
    
    // AI state management
    void SetAIState(EntityID enemy, EnemyAIState state);
    EnemyAIState GetAIState(EntityID enemy) const;
    
private:
    std::unordered_map<EntityID, AIBehavior> m_behaviors;
    std::unordered_map<EntityID, EnemyAIState> m_states;
};
```

## Physics API

### PhysicsEngine

Handles collision detection and physics simulation.

```cpp
class PhysicsEngine {
public:
    // Collision detection
    bool CheckCollision(const Vector3f& pos1, float radius1, 
                       const Vector3f& pos2, float radius2) const;
    
    bool CheckCollision(const BoundingBox& bbox1, 
                       const BoundingBox& bbox2) const;
    
    // Spatial queries
    std::vector<EntityID> GetEntitiesInRadius(const Vector3f& center, float radius) const;
    std::vector<EntityID> GetEntitiesInBox(const BoundingBox& box) const;
    
    // Physics simulation
    void Update(float deltaTime);
    void AddForce(EntityID entity, const Vector3f& force);
    void SetVelocity(EntityID entity, const Vector3f& velocity);
    
    // Spatial partitioning
    void RebuildSpatialIndex();
    void SetSpatialIndexEnabled(bool enabled);
    
private:
    std::unique_ptr<SpatialIndex> m_spatialIndex;
    std::vector<PhysicsBody> m_bodies;
    bool m_spatialIndexEnabled = true;
};
```

### CollisionSystem

Specialized collision detection for Tempest gameplay.

```cpp
class CollisionSystem {
public:
    // Player collisions
    bool CheckPlayerEnemyCollision(EntityID player, EntityID enemy) const;
    bool CheckPlayerProjectileCollision(EntityID player, EntityID projectile) const;
    bool CheckPlayerSpikeCollision(EntityID player, uint8_t segment) const;
    
    // Projectile collisions
    bool CheckProjectileEnemyCollision(EntityID projectile, EntityID enemy) const;
    std::vector<EntityID> GetEnemiesHitByProjectile(EntityID projectile) const;
    
    // Tube-specific collisions
    bool IsPositionInTube(const Vector3f& position) const;
    uint8_t GetSegmentAtPosition(const Vector3f& position) const;
    float GetDistanceAlongTube(const Vector3f& position) const;
    
    // Collision response
    void HandlePlayerEnemyCollision(EntityID player, EntityID enemy);
    void HandleProjectileEnemyCollision(EntityID projectile, EntityID enemy);
    
private:
    TubeGeometry m_tubeGeometry;
    float m_collisionTolerance = 0.05f;
};
```

## Configuration API

### GameConfig

Configuration system for game settings.

```cpp
struct GameConfig {
    // Graphics settings
    struct Graphics {
        uint32_t width = 1920;
        uint32_t height = 1080;
        bool fullscreen = false;
        bool vsync = true;
        std::string quality = "high";
        bool enableParticles = true;
        bool enablePostProcessing = true;
    } graphics;
    
    // Audio settings
    struct Audio {
        float masterVolume = 0.7f;
        float sfxVolume = 0.8f;
        float musicVolume = 0.6f;
        bool enableSpatialAudio = true;
        uint32_t sampleRate = 44100;
    } audio;
    
    // Input settings
    struct Input {
        KeyCode zapKey = KeyCode::Space;
        KeyCode fireKey = KeyCode::LeftControl;
        KeyCode leftKey = KeyCode::Left;
        KeyCode rightKey = KeyCode::Right;
        float mouseSensitivity = 1.0f;
        float gamepadSensitivity = 1.0f;
    } input;
    
    // Game settings
    struct Game {
        uint8_t lives = 3;
        std::string difficulty = "normal";
        uint32_t bonusLife = 20000;
        bool enableCheats = false;
        bool enableDebug = false;
    } game;
};
```

### ConfigManager

Manages configuration loading and saving.

```cpp
class ConfigManager {
public:
    // Configuration loading/saving
    bool LoadConfig(const std::string& path);
    bool SaveConfig(const std::string& path) const;
    
    // Configuration access
    const GameConfig& GetConfig() const;
    GameConfig& GetConfig();
    
    // Individual settings
    template<typename T>
    T GetSetting(const std::string& path) const;
    
    template<typename T>
    void SetSetting(const std::string& path, const T& value);
    
    // Default configuration
    void ResetToDefaults();
    void LoadDefaults();
    
    // Configuration validation
    bool ValidateConfig(const GameConfig& config) const;
    std::vector<std::string> GetValidationErrors() const;
    
private:
    GameConfig m_config;
    std::vector<std::string> m_validationErrors;
};
```

## Utility API

### Math Types

```cpp
// Vector types
struct Vector2f {
    float x, y;
    
    Vector2f() : x(0), y(0) {}
    Vector2f(float x, float y) : x(x), y(y) {}
    
    Vector2f operator+(const Vector2f& other) const;
    Vector2f operator-(const Vector2f& other) const;
    Vector2f operator*(float scalar) const;
    float Length() const;
    Vector2f Normalized() const;
};

struct Vector3f {
    float x, y, z;
    
    Vector3f() : x(0), y(0), z(0) {}
    Vector3f(float x, float y, float z) : x(x), y(y), z(z) {}
    
    Vector3f operator+(const Vector3f& other) const;
    Vector3f operator-(const Vector3f& other) const;
    Vector3f operator*(float scalar) const;
    float Length() const;
    Vector3f Normalized() const;
    Vector3f Cross(const Vector3f& other) const;
    float Dot(const Vector3f& other) const;
};

// Matrix types
struct Matrix4f {
    float m[16];
    
    Matrix4f();
    Matrix4f(float* data);
    
    static Matrix4f Identity();
    static Matrix4f Translation(const Vector3f& translation);
    static Matrix4f Rotation(float angle, const Vector3f& axis);
    static Matrix4f Scale(const Vector3f& scale);
    static Matrix4f Perspective(float fov, float aspect, float near, float far);
    static Matrix4f Orthographic(float left, float right, float bottom, float top, float near, float far);
    
    Matrix4f operator*(const Matrix4f& other) const;
    Vector3f operator*(const Vector3f& vector) const;
    Matrix4f Inverse() const;
    Matrix4f Transpose() const;
};

// Color type
struct Color {
    float r, g, b, a;
    
    Color() : r(1), g(1), b(1), a(1) {}
    Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
    
    static Color Red() { return Color(1, 0, 0); }
    static Color Green() { return Color(0, 1, 0); }
    static Color Blue() { return Color(0, 0, 1); }
    static Color White() { return Color(1, 1, 1); }
    static Color Black() { return Color(0, 0, 0); }
};
```

### Logger

Logging system for debugging and diagnostics.

```cpp
class Logger {
public:
    enum class Level { Debug, Info, Warning, Error, Fatal };
    
    // Logging methods
    template<typename... Args>
    static void Log(Level level, const char* format, Args... args);
    
    template<typename... Args>
    static void Debug(const char* format, Args... args);
    
    template<typename... Args>
    static void Info(const char* format, Args... args);
    
    template<typename... Args>
    static void Warning(const char* format, Args... args);
    
    template<typename... Args>
    static void Error(const char* format, Args... args);
    
    template<typename... Args>
    static void Fatal(const char* format, Args... args);
    
    // Configuration
    static void SetLogLevel(Level level);
    static void SetLogFile(const std::string& path);
    static void EnableConsoleOutput(bool enable);
    
private:
    static Level s_logLevel;
    static std::string s_logFile;
    static bool s_consoleOutput;
};
```

### Profiler

Performance profiling and timing utilities.

```cpp
class Profiler {
public:
    // Scoped profiling
    class Scope {
    public:
        Scope(const char* name);
        ~Scope();
        
    private:
        const char* m_name;
        std::chrono::high_resolution_clock::time_point m_start;
    };
    
    // Manual timing
    static void BeginSample(const char* name);
    static void EndSample(const char* name);
    
    // Statistics
    static void BeginFrame();
    static void EndFrame();
    static void Reset();
    
    // Reporting
    static void PrintStats();
    static void SaveStats(const std::string& path);
    
    // Configuration
    static void SetEnabled(bool enabled);
    static void SetMaxSamples(uint32_t maxSamples);
    
private:
    struct Sample {
        std::string name;
        std::chrono::microseconds totalTime;
        uint32_t callCount;
        std::chrono::microseconds minTime;
        std::chrono::microseconds maxTime;
    };
    
    static std::unordered_map<std::string, Sample> s_samples;
    static bool s_enabled;
    static uint32_t s_maxSamples;
};

#define PROFILE_SCOPE(name) Profiler::Scope scope##__LINE__(name)
#define PROFILE_BEGIN(name) Profiler::BeginSample(name)
#define PROFILE_END(name) Profiler::EndSample(name)
``` 