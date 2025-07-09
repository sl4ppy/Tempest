#pragma once

#include <memory>
#include <chrono>
#include <spdlog/spdlog.h>
#include "EntityManager.h"
#include "EventSystem.h"
#include "GameState.h"
#include "../Game/LevelManager.h"
#include "../Game/ScoreManager.h"

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

private:
    // Core systems
    EntityManager entityManager_;
    GameStateManager gameStateManager_;
    GameState gameState_;
    LevelManager levelManager_;
    ScoreManager scoreManager_;

    // Game loop state
    bool running_;
    float targetFPS_;
    float maxFrameTime_;
    float fps_;
    float frameTime_;
    
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