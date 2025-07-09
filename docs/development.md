# Development Guidelines

## Code Standards

### C++ Coding Standards

#### General Principles
- **Modern C++**: Use C++20 features where appropriate
- **RAII**: Resource Acquisition Is Initialization
- **SOLID Principles**: Single responsibility, open/closed, Liskov substitution, interface segregation, dependency inversion
- **DRY**: Don't Repeat Yourself
- **KISS**: Keep It Simple, Stupid

#### Naming Conventions
```cpp
// Classes and structs: PascalCase
class GameEngine { };
struct PlayerData { };

// Functions and methods: camelCase
void updateGameState();
bool isPlayerAlive();

// Variables: camelCase
int playerScore;
float enemySpeed;

// Constants: UPPER_SNAKE_CASE
const int MAX_ENEMIES = 100;
const float PLAYER_SPEED = 5.0f;

// Private members: m_camelCase
class Player {
private:
    int m_health;
    float m_position;
};

// Static members: s_camelCase
class GameManager {
private:
    static GameManager* s_instance;
};
```

#### File Organization
```cpp
// Header file structure (GameEngine.h)
#pragma once

// System includes
#include <vector>
#include <memory>

// Project includes
#include "Core/Types.h"
#include "Core/Component.h"

// Forward declarations
class Renderer;
class AudioManager;

// Class definition
class GameEngine {
public:
    // Public interface
    GameEngine();
    ~GameEngine();
    
    void Initialize();
    void Update(float deltaTime);
    void Shutdown();
    
private:
    // Private implementation
    void ProcessInput();
    void UpdateGameLogic();
    void Render();
    
    // Member variables
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<AudioManager> m_audioManager;
};
```

#### Memory Management
```cpp
// Prefer smart pointers over raw pointers
std::unique_ptr<GameObject> m_player;
std::shared_ptr<Texture> m_texture;
std::weak_ptr<GameObject> m_target;

// Use RAII for resource management
class ResourceManager {
public:
    ResourceManager() = default;
    ~ResourceManager() {
        // Automatic cleanup
    }
    
    // Disable copying
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    // Allow moving
    ResourceManager(ResourceManager&&) = default;
    ResourceManager& operator=(ResourceManager&&) = default;
};
```

#### Error Handling
```cpp
// Use exceptions for exceptional cases
class GameException : public std::exception {
public:
    explicit GameException(const std::string& message) 
        : m_message(message) {}
    
    const char* what() const noexcept override {
        return m_message.c_str();
    }
    
private:
    std::string m_message;
};

// Use std::optional for functions that might fail
std::optional<GameObject> FindEnemyAtPosition(const Vector2f& position);

// Use std::expected for functions that return values or errors
std::expected<Texture, LoadError> LoadTexture(const std::string& path);
```

### Project Structure

#### Directory Layout
```
src/
├── Core/                    # Core engine systems
│   ├── Types.h             # Common type definitions
│   ├── Component.h         # ECS component base
│   ├── Entity.h            # Entity management
│   └── System.h            # System base class
├── Game/                   # Game-specific code
│   ├── Player.h            # Player entity
│   ├── Enemy.h             # Enemy entities
│   ├── Level.h             # Level management
│   └── GameState.h         # Game state machine
├── Graphics/               # Rendering system
│   ├── Renderer.h          # Main renderer
│   ├── Shader.h            # Shader management
│   ├── VectorRenderer.h    # Vector graphics
│   └── ParticleSystem.h    # Particle effects
├── Audio/                  # Audio system
│   ├── AudioManager.h      # Main audio manager
│   ├── POKEYSynthesizer.h  # Sound synthesis
│   └── SoundEffect.h       # Sound effect management
├── Input/                  # Input handling
│   ├── InputManager.h      # Main input manager
│   ├── KeyboardHandler.h   # Keyboard input
│   └── GamepadHandler.h    # Gamepad input
└── Platform/               # Platform-specific code
    ├── Windows/            # Windows implementation
    ├── macOS/              # macOS implementation
    ├── Linux/              # Linux implementation
    └── Web/                # Web implementation
```

#### Header File Guidelines
```cpp
// Always use include guards or #pragma once
#pragma once

// Include order:
// 1. Current header's corresponding .cpp file
// 2. System headers
// 3. Third-party library headers
// 4. Project headers

#include <vector>
#include <memory>
#include <optional>

#include "Core/Types.h"
#include "Core/Component.h"

// Forward declarations for classes not used in interface
class Renderer;
class AudioManager;

// Class definition
class GameEngine {
    // Implementation...
};
```

### Documentation Standards

#### Code Comments
```cpp
/**
 * @brief Manages the main game loop and systems
 * 
 * The GameEngine class coordinates all major game systems including
 * rendering, audio, input, and game logic. It implements the main
 * game loop and manages the overall game state.
 * 
 * @example
 * ```cpp
 * GameEngine engine;
 * engine.Initialize();
 * while (engine.IsRunning()) {
 *     engine.Update(deltaTime);
 * }
 * engine.Shutdown();
 * ```
 */
class GameEngine {
public:
    /**
     * @brief Initializes the game engine and all subsystems
     * 
     * This method must be called before any other operations.
     * It initializes the rendering, audio, and input systems.
     * 
     * @throws GameException if initialization fails
     * @return true if initialization successful, false otherwise
     */
    bool Initialize();
    
    /**
     * @brief Updates the game state for one frame
     * 
     * Processes input, updates game logic, and renders the frame.
     * This method should be called once per frame at 60 FPS.
     * 
     * @param deltaTime Time elapsed since last frame in seconds
     */
    void Update(float deltaTime);
    
private:
    std::unique_ptr<Renderer> m_renderer;    ///< Main rendering system
    std::unique_ptr<AudioManager> m_audio;   ///< Audio management system
    bool m_isRunning;                        ///< Engine running state
};
```

#### API Documentation
```cpp
/**
 * @file GameEngine.h
 * @brief Main game engine interface
 * @author Your Name
 * @date 2024-01-01
 * @version 1.0.0
 * 
 * This header defines the main GameEngine class that coordinates
 * all game systems and implements the main game loop.
 */

/**
 * @namespace Tempest
 * @brief Main namespace for the Tempest game engine
 * 
 * All public APIs are contained within this namespace to avoid
 * naming conflicts with other libraries.
 */
namespace Tempest {
    // Class definitions...
}
```

## Development Workflow

### Git Workflow

#### Branch Strategy
```
main                    # Production-ready code
├── develop            # Integration branch
├── feature/player-ai  # Feature branches
├── bugfix/crash-fix   # Bug fix branches
└── release/v1.0.0     # Release branches
```

#### Commit Message Format
```
type(scope): description

[optional body]

[optional footer]
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code refactoring
- `test`: Test additions/changes
- `chore`: Build/tool changes

**Examples:**
```
feat(player): add player movement system

- Implement WASD movement controls
- Add collision detection
- Add movement animations

Closes #123
```

```
fix(rendering): fix vector graphics scaling

The vector graphics were not scaling correctly on high DPI displays.
This fix ensures proper scaling across all resolutions.

Fixes #456
```

#### Pull Request Process
1. **Create Feature Branch**: `git checkout -b feature/new-feature`
2. **Make Changes**: Implement feature with tests
3. **Commit Changes**: Use conventional commit format
4. **Push Branch**: `git push origin feature/new-feature`
5. **Create PR**: Submit pull request to develop branch
6. **Code Review**: Address review comments
7. **Merge**: Merge after approval

### Testing Strategy

#### Unit Testing
```cpp
// Use Google Test framework
#include <gtest/gtest.h>

class PlayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_player = std::make_unique<Player>();
    }
    
    void TearDown() override {
        m_player.reset();
    }
    
    std::unique_ptr<Player> m_player;
};

TEST_F(PlayerTest, MovementTest) {
    // Arrange
    Vector2f startPos = m_player->GetPosition();
    
    // Act
    m_player->Move(Vector2f(1.0f, 0.0f));
    
    // Assert
    Vector2f endPos = m_player->GetPosition();
    EXPECT_EQ(endPos.x, startPos.x + 1.0f);
    EXPECT_EQ(endPos.y, startPos.y);
}
```

#### Integration Testing
```cpp
class GameEngineIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_engine = std::make_unique<GameEngine>();
        m_engine->Initialize();
    }
    
    void TearDown() override {
        m_engine->Shutdown();
    }
    
    std::unique_ptr<GameEngine> m_engine;
};

TEST_F(GameEngineIntegrationTest, FullGameLoop) {
    // Test complete game loop
    m_engine->Update(1.0f / 60.0f);
    
    // Verify systems are working together
    EXPECT_TRUE(m_engine->IsRunning());
}
```

#### Performance Testing
```cpp
TEST(PerformanceTest, FrameTimeBudget) {
    GameEngine engine;
    engine.Initialize();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Run 1000 frames
    for (int i = 0; i < 1000; ++i) {
        engine.Update(1.0f / 60.0f);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete 1000 frames in under 17 seconds (60 FPS)
    EXPECT_LT(duration.count(), 17000);
}
```

### Build System

#### CMake Configuration
```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(Tempest VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(ENABLE_VULKAN "Enable Vulkan renderer" ON)

# Find dependencies
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenAL REQUIRED)

# Add subdirectories
add_subdirectory(src)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
```

#### Platform-Specific Builds
```cmake
# Platform detection
if(WIN32)
    set(PLATFORM_SOURCES src/Platform/Windows/WindowsPlatform.cpp)
elseif(APPLE)
    set(PLATFORM_SOURCES src/Platform/macOS/MacOSPlatform.cpp)
elseif(UNIX)
    set(PLATFORM_SOURCES src/Platform/Linux/LinuxPlatform.cpp)
endif()

# Graphics API selection
if(ENABLE_VULKAN AND VULKAN_FOUND)
    set(GRAPHICS_API "Vulkan")
    set(GRAPHICS_SOURCES src/Graphics/VulkanRenderer.cpp)
else()
    set(GRAPHICS_API "OpenGL")
    set(GRAPHICS_SOURCES src/Graphics/OpenGLRenderer.cpp)
endif()
```

### Code Quality Tools

#### Static Analysis
```yaml
# .clang-tidy
Checks: '-*,modernize-*,performance-*,readability-*,bugprone-*'
WarningsAsErrors: ''
HeaderFilterRegex: '.*'
AnalyzeTemporaryDtors: true
CheckOptions:
  - key: modernize-use-nullptr
    value: '1'
  - key: modernize-use-override
    value: '1'
```

#### Code Formatting
```json
// .clang-format
{
  "BasedOnStyle": "Google",
  "IndentWidth": 4,
  "TabWidth": 4,
  "UseTab": "Never",
  "ColumnLimit": 100,
  "AccessModifierOffset": -4,
  "AlignConsecutiveAssignments": true,
  "AlignConsecutiveDeclarations": true
}
```

#### Pre-commit Hooks
```yaml
# .pre-commit-config.yaml
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.4.0
    hooks:
      - id: trailing-whitespace
      - id: end-of-file-fixer
      - id: check-yaml
      - id: check-added-large-files
  
  - repo: https://github.com/psf/black
    rev: 23.3.0
    hooks:
      - id: black
  
  - repo: local
    hooks:
      - id: clang-format
        name: clang-format
        entry: clang-format
        language: system
        types: [c++]
```

## Performance Guidelines

### Optimization Principles
1. **Profile First**: Measure before optimizing
2. **Cache-Friendly**: Optimize for CPU cache
3. **Memory Access**: Minimize memory allocations
4. **Algorithm Choice**: Use appropriate algorithms
5. **Parallelization**: Use multiple threads where safe

### Memory Management
```cpp
// Use object pools for frequently allocated objects
class GameObjectPool {
public:
    GameObject* Allocate() {
        if (m_freeList.empty()) {
            return new GameObject();
        }
        GameObject* obj = m_freeList.back();
        m_freeList.pop_back();
        return obj;
    }
    
    void Deallocate(GameObject* obj) {
        obj->Reset();
        m_freeList.push_back(obj);
    }
    
private:
    std::vector<GameObject*> m_freeList;
};

// Use memory arenas for temporary allocations
class MemoryArena {
public:
    void* Allocate(size_t size) {
        if (m_currentOffset + size > m_buffer.size()) {
            // Allocate new buffer
            m_buffer.resize(m_buffer.size() * 2);
        }
        void* ptr = &m_buffer[m_currentOffset];
        m_currentOffset += size;
        return ptr;
    }
    
    void Reset() {
        m_currentOffset = 0;
    }
    
private:
    std::vector<uint8_t> m_buffer;
    size_t m_currentOffset = 0;
};
```

### Rendering Optimization
```cpp
// Batch rendering commands
class RenderBatch {
public:
    void AddCommand(const RenderCommand& cmd) {
        m_commands.push_back(cmd);
    }
    
    void Flush() {
        // Sort commands by shader, texture, etc.
        std::sort(m_commands.begin(), m_commands.end());
        
        // Execute in batches
        for (const auto& cmd : m_commands) {
            ExecuteCommand(cmd);
        }
        
        m_commands.clear();
    }
    
private:
    std::vector<RenderCommand> m_commands;
};

// Use frustum culling
class FrustumCuller {
public:
    bool IsVisible(const BoundingBox& bbox) const {
        for (int i = 0; i < 6; ++i) {
            if (!m_planes[i].IsOnPositiveSide(bbox)) {
                return false;
            }
        }
        return true;
    }
    
private:
    std::array<Plane, 6> m_planes;
};
```

## Debugging Guidelines

### Debug Tools
```cpp
// Debug logging system
class Logger {
public:
    enum class Level { Debug, Info, Warning, Error };
    
    template<typename... Args>
    static void Log(Level level, const char* format, Args... args) {
        if (level >= s_minLevel) {
            printf("[%s] ", GetLevelString(level));
            printf(format, args...);
            printf("\n");
        }
    }
    
private:
    static Level s_minLevel;
};

// Performance profiling
class Profiler {
public:
    class Scope {
    public:
        Scope(const char* name) : m_name(name), m_start(std::chrono::high_resolution_clock::now()) {}
        ~Scope() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start);
            Logger::Log(Logger::Level::Debug, "%s: %lld us", m_name, duration.count());
        }
    private:
        const char* m_name;
        std::chrono::high_resolution_clock::time_point m_start;
    };
};

#define PROFILE_SCOPE(name) Profiler::Scope scope##__LINE__(name)
```

### Debug Features
```cpp
// Debug rendering
class DebugRenderer {
public:
    void DrawBoundingBox(const BoundingBox& bbox, const Color& color);
    void DrawLine(const Vector3f& start, const Vector3f& end, const Color& color);
    void DrawText(const Vector3f& position, const std::string& text);
    
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
private:
    bool m_enabled = false;
};

// Debug commands
class DebugConsole {
public:
    void RegisterCommand(const std::string& name, std::function<void(const std::vector<std::string>&)> func);
    void ExecuteCommand(const std::string& command);
    
private:
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> m_commands;
};
```

## Release Process

### Version Management
```cpp
// Version information
struct Version {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
    std::string build;
    
    std::string ToString() const {
        return std::to_string(major) + "." + 
               std::to_string(minor) + "." + 
               std::to_string(patch) + 
               (build.empty() ? "" : "-" + build);
    }
};

const Version CURRENT_VERSION = {1, 0, 0, "alpha"};
```

### Release Checklist
- [ ] All tests passing
- [ ] Performance benchmarks met
- [ ] Documentation updated
- [ ] Version numbers updated
- [ ] Release notes written
- [ ] Assets finalized
- [ ] Platform-specific builds tested
- [ ] Installer packages created
- [ ] Distribution packages signed

### Distribution
```cmake
# CPack configuration for installer generation
set(CPACK_PACKAGE_NAME "Tempest")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tempest Arcade Game Rebuild")
set(CPACK_PACKAGE_VENDOR "Your Organization")

# Windows installer
if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "Tempest")
    set(CPACK_NSIS_CONTACT "your.email@example.com")
endif()

# macOS package
if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "Tempest")
endif()

# Linux package
if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name")
endif()
``` 