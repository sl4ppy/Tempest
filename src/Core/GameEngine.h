#pragma once

#include <memory>
#include <chrono>
#include <spdlog/spdlog.h>
#include "EntityManager.h"
#include "EventSystem.h"
#include "GameState.h"
#include "../Game/LevelManager.h"
#include "../Game/ScoreManager.h"
#include "../Game/Player.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/TubeRenderer.h"
#include "../Game/TubeGeometry.h"
#include "Graphics/VectorCommand.h"
#include "Graphics/TempestShapes.h"
#include "Audio/AudioManager.h"
#include "Input/InputManager.h"
#include "Platform/PlatformManager.h"

// Forward declaration
struct SDL_Renderer;
struct SDL_Window;
#ifdef USE_SDL
#include <SDL.h>
#else
// Forward declarations for test builds
typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;
#endif

namespace Tempest {

/**
 * @brief Main Game Engine class
 * 
 * Coordinates all core systems and manages the main game loop.
 * Based on the original game's architecture but with modern C++ features.
 */
class GameEngine {
public:
    GameEngine();
    ~GameEngine() = default;

    // Engine lifecycle
    bool initialize();
    void shutdown();
    void run();
    void stop();

    // Core system access
    EntityManager& getEntityManager() { return entityManager_; }
    EventSystem& getEventSystem() { return EventSystem::getInstance(); }
    GameStateManager& getGameStateManager() { return gameStateManager_; }
    GameState& getGameState() { return gameState_; }
    LevelManager& getLevelManager() { return levelManager_; }
    ScoreManager& getScoreManager() { return scoreManager_; }
    Player* getPlayer() { return player_.get(); }
    Renderer* getRenderer() { return renderer_.get(); }
    TubeRenderer* getTubeRenderer() { return tubeRenderer_.get(); }

    // Game loop control
    void update(float deltaTime);
    void render();
    void processEvents();

    // Performance monitoring
    float getFPS() const { return fps_; }
    float getFrameTime() const { return frameTime_; }
    size_t getEntityCount() const { return entityManager_.getEntityCount(); }

    // Configuration
    void setTargetFPS(float fps) { targetFPS_ = fps; }
    void setMaxFrameTime(float maxFrameTime) { maxFrameTime_ = maxFrameTime; }
    void setGraphicsAPI(const std::string& api) { graphicsAPI_ = api; }

    // Rendering methods
    void setSDLWindow(SDL_Window* window) { sdlWindow_ = window; }
    void setGLContext(SDL_GLContext context) { glContext_ = context; }
    SDL_GLContext glContext_;
    
    // Audio methods

private:
    // Core systems
    EntityManager entityManager_;
    GameStateManager gameStateManager_;
    GameState gameState_;
    LevelManager levelManager_;
    ScoreManager scoreManager_;
    
    // Game objects
    std::unique_ptr<Player> player_;

    // Rendering systems
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<TubeRenderer> tubeRenderer_;
    std::unique_ptr<TubeGeometry> tubeGeometry_;
    std::unique_ptr<VectorRenderer> vectorRenderer_;
    std::unique_ptr<VectorCommandProcessor> vectorProcessor_;
    std::unique_ptr<TempestShapes> tempestShapes_;
    
    // SDL window and OpenGL context handle for buffer swapping
    SDL_Window* sdlWindow_;
    
    // Audio systems
    std::unique_ptr<AudioManager> audioManager_;

    // Game loop state
    bool running_;
    float targetFPS_;
    float maxFrameTime_;
    float fps_;
    float frameTime_;
    
    // Graphics configuration
    std::string graphicsAPI_;
    
    // Timing
    std::chrono::high_resolution_clock::time_point lastFrameTime_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    // Performance tracking
    float frameTimeAccumulator_;
    int frameCount_;
    float fpsUpdateTimer_;

    // Engine methods
    void updateTiming();
    void updatePerformanceMetrics(float deltaTime);
    void handleSystemEvents();
    void handleKeyDown(int key);
    void handleKeyUp(int key);
    bool initializeRendering();
    void shutdownRendering();
    void initializeVectorRenderer();
};

/**
 * @brief Game Engine singleton for global access
 */
class GameEngineSingleton {
public:
    static GameEngine& getInstance();
    
    static bool initialize() {
        return getInstance().initialize();
    }
    
    static void shutdown() {
        getInstance().shutdown();
    }
    
    static void run() {
        getInstance().run();
    }
    
    static void stop() {
        getInstance().stop();
    }

private:
    GameEngineSingleton() = default;
    ~GameEngineSingleton() = default;
    GameEngineSingleton(const GameEngineSingleton&) = delete;
    GameEngineSingleton& operator=(const GameEngineSingleton&) = delete;
};

} // namespace Tempest 