#include "GameEngine.h"
#include <thread>
#include <chrono>

namespace Tempest {

GameEngine::GameEngine() 
    : running_(false), targetFPS_(60.0f), maxFrameTime_(0.033f), fps_(0.0f), frameTime_(0.0f),
      frameTimeAccumulator_(0.0f), frameCount_(0), fpsUpdateTimer_(0.0f) {
    
    lastFrameTime_ = std::chrono::high_resolution_clock::now();
    startTime_ = lastFrameTime_;
    
    spdlog::info("GameEngine constructed");
}

bool GameEngine::initialize() {
    spdlog::info("Initializing GameEngine...");
    
    // Initialize core systems
    // Note: EntityManager and GameStateManager are initialized in their constructors
    
    // Initialize level and score managers
    levelManager_.initialize();
    scoreManager_.initialize();
    
    // Subscribe to game state changes
    auto& eventSystem = EventSystem::getInstance();
    eventSystem.subscribe<GameStateChangedEvent>(
        [this](const GameStateChangedEvent& event) {
            gameStateManager_.handleGameStateChanged(event);
        }
    );
    
    // Subscribe to level and score events
    eventSystem.subscribe<LevelCompletedEvent>(
        [this](const LevelCompletedEvent& event) {
            levelManager_.completeLevel();
            scoreManager_.handleLevelCompleted(event);
        }
    );
    
    eventSystem.subscribe<LevelFailedEvent>(
        [this](const LevelFailedEvent& event) {
            levelManager_.failLevel(event.failureReason);
            scoreManager_.handlePlayerDeath(event);
        }
    );
    
    spdlog::info("GameEngine initialized successfully");
    return true;
}

void GameEngine::shutdown() {
    spdlog::info("Shutting down GameEngine...");
    running_ = false;
    
    // Clean up systems
    EventSystem::getInstance().clearQueue();
    
    spdlog::info("GameEngine shutdown complete");
}

void GameEngine::run() {
    if (running_) {
        spdlog::warn("GameEngine is already running");
        return;
    }
    
    spdlog::info("Starting GameEngine main loop");
    running_ = true;
    
    while (running_) {
        updateTiming();
        
        float deltaTime = frameTime_;
        if (deltaTime > maxFrameTime_) {
            deltaTime = maxFrameTime_;
        }
        
        // Process events
        processEvents();
        
        // Update game logic
        update(deltaTime);
        
        // Render frame
        render();
        
        // Update performance metrics
        updatePerformanceMetrics(deltaTime);
        
        // Frame rate limiting
        if (targetFPS_ > 0.0f) {
            float targetFrameTime = 1.0f / targetFPS_;
            float elapsed = frameTime_;
            
            if (elapsed < targetFrameTime) {
                float sleepTime = targetFrameTime - elapsed;
                std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
            }
        }
    }
    
    spdlog::info("GameEngine main loop ended");
}

void GameEngine::stop() {
    spdlog::info("Stopping GameEngine");
    running_ = false;
}

void GameEngine::update(float deltaTime) {
    // Update game state manager
    gameStateManager_.update(deltaTime);
    
    // Update level and score managers
    levelManager_.update(deltaTime);
    scoreManager_.update(deltaTime);
    
    // Update entity systems (to be implemented)
    // entityManager_.update(deltaTime);
    
    // Process event queue
    EventSystem::getInstance().processQueue();
    
    // Update performance metrics
    frameTimeAccumulator_ += deltaTime;
    frameCount_++;
}

void GameEngine::render() {
    // Render systems (to be implemented)
    // This will be handled by the graphics system
}

void GameEngine::processEvents() {
    // Process system events (to be implemented)
    // This will handle input, window events, etc.
    handleSystemEvents();
}

void GameEngine::updateTiming() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime_).count();
    
    frameTime_ = deltaTime;
    lastFrameTime_ = currentTime;
}

void GameEngine::updatePerformanceMetrics(float deltaTime) {
    fpsUpdateTimer_ += deltaTime;
    
    // Update FPS every second
    if (fpsUpdateTimer_ >= 1.0f) {
        fps_ = static_cast<float>(frameCount_) / fpsUpdateTimer_;
        frameCount_ = 0;
        fpsUpdateTimer_ = 0.0f;
        
        spdlog::debug("FPS: {:.1f}, Frame Time: {:.3f}ms, Entities: {}", 
                     fps_, frameTime_ * 1000.0f, getEntityCount());
    }
}

void GameEngine::handleSystemEvents() {
    // Handle system-level events (to be implemented)
    // This will include window events, input events, etc.
}

// GameEngineSingleton implementation
GameEngine& GameEngineSingleton::getInstance() {
    static GameEngine instance;
    return instance;
}

} // namespace Tempest 