#ifdef USE_SDL
#include <SDL.h>
#endif
#include "GameEngine.h"
#include "Graphics/glad/gl.h"
#include <spdlog/spdlog.h>
#include "Graphics/Camera.h"
#include "Graphics/GraphicsAPI.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <SDL.h>

namespace Tempest {

GameEngine::GameEngine() 
    : running_(false), targetFPS_(60.0f), maxFrameTime_(0.033f), fps_(0.0f), frameTime_(0.0f),
      frameTimeAccumulator_(0.0f), frameCount_(0), fpsUpdateTimer_(0.0f), sdlWindow_(nullptr) {
    spdlog::info(">>> Entering GameEngine constructor");
    
    lastFrameTime_ = std::chrono::high_resolution_clock::now();
    startTime_ = lastFrameTime_;
    
    spdlog::info("GameEngine constructed");
    spdlog::info(">>> Exiting GameEngine constructor");
}

bool GameEngine::initialize() {
    spdlog::info("Initializing GameEngine...");
    
    // Initialize rendering systems
    if (!initializeRendering()) {
        spdlog::error("Failed to initialize rendering systems");
        return false;
    }
    
    // Initialize core systems
    // Note: EntityManager and GameStateManager are initialized in their constructors
    
    // Initialize level and score managers
    levelManager_.initialize();
    scoreManager_.initialize();
    
    // Initialize player
    player_ = std::make_unique<Player>();
    player_->initialize();
    
    // Subscribe to game state changes
    auto& eventSystem = EventSystem::getInstance();
    eventSystem.subscribe<GameStateChangedEvent>(
        [this](const GameStateChangedEvent& event) {
            gameStateManager_.handleGameStateChanged(event);
        }
    );
    
    // Subscribe to player input events
    eventSystem.subscribe<PlayerInputEvent>(
        [this](const PlayerInputEvent& event) {
            if (player_) {
                player_->handleInput(event);
            }
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
    
    // Shutdown player
    if (player_) {
        player_->shutdown();
        player_.reset();
    }
    
    // Shutdown rendering systems
    shutdownRendering();
    
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
        spdlog::info("Main loop running...");
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
        // Add a small delay for visibility
#ifdef USE_SDL
        SDL_Delay(1);
#endif
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
    
    // Update player
    if (player_) {
        player_->update(deltaTime);
    }
    
    // Update entity systems (to be implemented)
    // entityManager_.update(deltaTime);
    
    // Process event queue
    EventSystem::getInstance().processQueue();
    
    // Update performance metrics
    frameTimeAccumulator_ += deltaTime;
    frameCount_++;
}

void GameEngine::render() {
    if (!renderer_) return;
    
    // Begin frame (clears screen)
    renderer_->BeginFrame();
    
    // Set clear color to dark gray so we can see if clearing is working
    renderer_->SetClearColor(Color(0.2f, 0.2f, 0.2f, 1.0f));
    
    // Initialize vector renderer if not already done
    if (!vectorRenderer_) {
        initializeVectorRenderer();
    }
    
    // Test the VectorCommand system with classic Tempest shapes
    if (vectorProcessor_ && tempestShapes_) {
        // Initialize vector renderer if not already done
        if (!vectorRenderer_) {
            spdlog::info("VectorRenderer not initialized, initializing now...");
            initializeVectorRenderer();
        }
        
        if (!vectorRenderer_) {
            spdlog::error("VectorRenderer initialization failed!");
            return;
        }
        
        // Clear previous commands
        vectorProcessor_->ClearCommands();
        vectorProcessor_->ResetState();
        
        // Set clear color to black to make colored lines visible
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        // Set up orthographic projection for 2D vector graphics
        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        
        vectorRenderer_->SetProjectionMatrix(projection);
        vectorRenderer_->SetViewMatrix(view);
        vectorRenderer_->SetModelMatrix(model);
        
        // Get frame counter for animation
        static int frameCounter = 0;
        frameCounter++;
        
        // Create comprehensive TempestShapes showcase
        
        // 1. Player Ship (center)
        auto playerShip = tempestShapes_->CreatePlayerShip(10.0f);
        for (auto& cmd : playerShip) {
            vectorProcessor_->GetCommands().push_back(cmd);
        }
        
        // 2. Spiker Enemy (top left) - animated
        int spikerFrame = (frameCounter / 10) % 4;
        auto spikerEnemy = tempestShapes_->CreateSpiker(5.0f, spikerFrame);
        for (auto& cmd : spikerEnemy) {
            if (cmd.points.size() >= 2) {
                cmd.points[0] = glm::vec2(cmd.points[0].x - 0.6f, cmd.points[0].y + 0.6f);
                cmd.points[1] = glm::vec2(cmd.points[1].x - 0.6f, cmd.points[1].y + 0.6f);
            }
            vectorProcessor_->GetCommands().push_back(cmd);
        }
        
        // 3. Fuzzball Enemy (top right) - animated
        int fuzzballFrame = (frameCounter / 15) % 4;
        auto fuzzballEnemy = tempestShapes_->CreateFuzzball(5.0f, fuzzballFrame);
        for (auto& cmd : fuzzballEnemy) {
            if (cmd.points.size() >= 2) {
                cmd.points[0] = glm::vec2(cmd.points[0].x + 0.6f, cmd.points[0].y + 0.6f);
                cmd.points[1] = glm::vec2(cmd.points[1].x + 0.6f, cmd.points[1].y + 0.6f);
            }
            vectorProcessor_->GetCommands().push_back(cmd);
        }
        
        // 4. Tanker Enemy (bottom left)
        auto tankerEnemy = tempestShapes_->CreateTanker(5.0f, 0);
        for (auto& cmd : tankerEnemy) {
            if (cmd.points.size() >= 2) {
                cmd.points[0] = glm::vec2(cmd.points[0].x - 0.6f, cmd.points[0].y - 0.6f);
                cmd.points[1] = glm::vec2(cmd.points[1].x - 0.6f, cmd.points[1].y - 0.6f);
            }
            vectorProcessor_->GetCommands().push_back(cmd);
        }
        
        // 5. Pulsar Enemy (bottom right)
        auto pulsarEnemy = tempestShapes_->CreatePulsar(5.0f);
        for (auto& cmd : pulsarEnemy) {
            if (cmd.points.size() >= 2) {
                cmd.points[0] = glm::vec2(cmd.points[0].x + 0.6f, cmd.points[0].y - 0.6f);
                cmd.points[1] = glm::vec2(cmd.points[1].x + 0.6f, cmd.points[1].y - 0.6f);
            }
            vectorProcessor_->GetCommands().push_back(cmd);
        }
        
        // 6. Enemy Shot (animated position)
        int shotFrame = (frameCounter / 8) % 4;
        float shotX = 0.4f * sin(frameCounter * 0.05f);
        float shotY = 0.3f * cos(frameCounter * 0.05f);
        auto enemyShot = tempestShapes_->CreateEnemyShot(3.0f, shotFrame);
        for (auto& cmd : enemyShot) {
            if (cmd.points.size() >= 2) {
                cmd.points[0] = glm::vec2(cmd.points[0].x + shotX, cmd.points[0].y + shotY);
                cmd.points[1] = glm::vec2(cmd.points[1].x + shotX, cmd.points[1].y + shotY);
            }
            vectorProcessor_->GetCommands().push_back(cmd);
        }
        
        // 7. Super Zapper (bottom center)
        auto superZapper = tempestShapes_->CreateSuperZapper(4.0f);
        for (auto& cmd : superZapper) {
            if (cmd.points.size() >= 2) {
                cmd.points[0] = glm::vec2(cmd.points[0].x, cmd.points[0].y - 0.8f);
                cmd.points[1] = glm::vec2(cmd.points[1].x, cmd.points[1].y - 0.8f);
            }
            vectorProcessor_->GetCommands().push_back(cmd);
        }
        
        // 8. Flipper Enemy (left side)
        auto flipperEnemy = tempestShapes_->CreateFlipper(5.0f);
        for (auto& cmd : flipperEnemy) {
            if (cmd.points.size() >= 2) {
                cmd.points[0] = glm::vec2(cmd.points[0].x - 0.8f, cmd.points[0].y);
                cmd.points[1] = glm::vec2(cmd.points[1].x - 0.8f, cmd.points[1].y);
            }
            vectorProcessor_->GetCommands().push_back(cmd);
        }
        
        // 9. Explosion Effect (right side) - animated
        int explosionFrame = (frameCounter / 5) % 8;
        auto explosion = tempestShapes_->CreateExplosion(6.0f, explosionFrame);
        for (auto& cmd : explosion) {
            if (cmd.points.size() >= 2) {
                cmd.points[0] = glm::vec2(cmd.points[0].x + 0.8f, cmd.points[0].y);
                cmd.points[1] = glm::vec2(cmd.points[1].x + 0.8f, cmd.points[1].y);
            }
            vectorProcessor_->GetCommands().push_back(cmd);
        }
        
        // Render all commands
        const auto& commands = vectorProcessor_->GetCommands();
        vectorRenderer_->RenderCommands(commands);
        
        // Debug output every 60 frames
        if (frameCounter % 60 == 0) {
            spdlog::info("TEMPEST SHAPES: Rendering classic shapes, {} commands total", commands.size());
            spdlog::info("VectorRenderer initialized: {}, FPS: {:.1f}", vectorRenderer_ ? "YES" : "NO", fps_);
        }
    }
    
    // End frame and swap buffers
    renderer_->EndFrame();
    
    // Ensure buffer swapping happens
    if (sdlWindow_) {
        SDL_GL_SwapWindow(sdlWindow_);
    }
}

bool GameEngine::initializeRendering() {
    spdlog::info("Initializing rendering systems...");
    
    try {
        // Create renderer
        spdlog::info("Creating renderer...");
        renderer_ = std::make_unique<Renderer>();
        spdlog::info("Renderer created, initializing...");
        
        // Create graphics configuration
        GraphicsConfig config;
        config.api = graphicsAPI_.empty() ? "OpenGL" : graphicsAPI_;
        config.enableDebug = true;
        
        if (!renderer_->Initialize(config)) {
            spdlog::error("Failed to initialize renderer");
            return false;
        }
        
        // Create tube geometry
        tubeGeometry_ = std::make_unique<TubeGeometry>();
        tubeGeometry_->initialize();
        
        // Create tube renderer
        tubeRenderer_ = std::make_unique<TubeRenderer>();
        if (!tubeRenderer_->Initialize(renderer_.get())) {
            spdlog::error("Failed to initialize tube renderer");
            return false;
        }
        
        // Configure tube renderer
        tubeRenderer_->SetCameraPosition(glm::vec3(0.0f, -15.0f, 0.0f));
        tubeRenderer_->SetCameraTarget(glm::vec3(0.0f, 0.0f, 0.0f));
        tubeRenderer_->SetTubeRadius(1.0f);
        tubeRenderer_->SetTubeDepth(10.0f);
        tubeRenderer_->EnableDepthTest(true);
        tubeRenderer_->EnableBlending(true);
        
        // Create vector command processor (no OpenGL resources needed)
        vectorProcessor_ = std::make_unique<VectorCommandProcessor>();
        
        // Create Tempest shapes generator
        tempestShapes_ = std::make_unique<TempestShapes>();
        
        spdlog::info("Rendering systems initialized successfully");
        return true;
    }
    catch (const std::exception& e) {
        spdlog::error("Rendering initialization failed: {}", e.what());
        return false;
    }
}

void GameEngine::initializeVectorRenderer() {
    if (vectorRenderer_) return; // Already initialized
    
    vectorRenderer_ = std::make_unique<VectorRenderer>();
    if (!vectorRenderer_->Initialize()) {
        spdlog::error("Failed to initialize vector renderer");
        return;
    }
    
    // Configure vector renderer
    if (renderer_) {
        vectorRenderer_->SetProjectionMatrix(renderer_->GetProjectionMatrix());
        vectorRenderer_->SetViewMatrix(renderer_->GetViewMatrix());
        vectorRenderer_->SetModelMatrix(renderer_->GetModelMatrix());
    }
    
    spdlog::info("Vector renderer initialized successfully");
}

void GameEngine::shutdownRendering() {
    spdlog::info("Shutting down rendering systems...");
    
    if (vectorRenderer_) {
        vectorRenderer_->Shutdown();
        vectorRenderer_.reset();
    }
    
    if (vectorProcessor_) {
        vectorProcessor_->ClearCommands();
        vectorProcessor_.reset();
    }
    
    if (tempestShapes_) {
        tempestShapes_.reset();
    }
    
    if (tubeRenderer_) {
        tubeRenderer_->Shutdown();
        tubeRenderer_.reset();
    }
    
    if (renderer_) {
        renderer_->Shutdown();
        renderer_.reset();
    }
    
    tubeGeometry_.reset();
    
    spdlog::info("Rendering systems shut down");
}

void GameEngine::processEvents() {
    // Process system events
    handleSystemEvents();
}

void GameEngine::handleSystemEvents() {
#ifdef USE_SDL
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                spdlog::info("SDL_QUIT event received, stopping main loop");
                running_ = false;
                break;
                
            case SDL_KEYDOWN:
                handleKeyDown(event.key.keysym.sym);
                break;
                
            case SDL_KEYUP:
                handleKeyUp(event.key.keysym.sym);
                break;
                
            default:
                break;
        }
    }
#endif
}

void GameEngine::handleKeyDown(int key) {
    PlayerInputEvent::InputType inputType = PlayerInputEvent::InputType::MoveLeft; // Default initialization
    bool validInput = false;
    
#ifdef USE_SDL
    switch (key) {
        case SDLK_LEFT:
        case SDLK_a:
            inputType = PlayerInputEvent::InputType::MoveLeft;
            validInput = true;
            break;
            
        case SDLK_RIGHT:
        case SDLK_d:
            inputType = PlayerInputEvent::InputType::MoveRight;
            validInput = true;
            break;
            
        case SDLK_SPACE:
        case SDLK_z:
            inputType = PlayerInputEvent::InputType::Zap;
            validInput = true;
            break;
            
        case SDLK_LCTRL:
        case SDLK_RCTRL:
        case SDLK_x:
            inputType = PlayerInputEvent::InputType::Fire;
            validInput = true;
            break;
            
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
        case SDLK_c:
            inputType = PlayerInputEvent::InputType::SuperZapper;
            validInput = true;
            break;
            
        default:
            break;
    }
#endif
    
    if (validInput) {
        PlayerInputEvent inputEvent(inputType, true);
        EventSystem::getInstance().dispatch(inputEvent);
        spdlog::debug("Key pressed: {}, Input type: {}", key, static_cast<int>(inputType));
    }
}

void GameEngine::handleKeyUp(int key) {
    PlayerInputEvent::InputType inputType = PlayerInputEvent::InputType::MoveLeft; // Default initialization
    bool validInput = false;
    
#ifdef USE_SDL
    switch (key) {
        case SDLK_LEFT:
        case SDLK_a:
            inputType = PlayerInputEvent::InputType::MoveLeft;
            validInput = true;
            break;
            
        case SDLK_RIGHT:
        case SDLK_d:
            inputType = PlayerInputEvent::InputType::MoveRight;
            validInput = true;
            break;
            
        case SDLK_SPACE:
        case SDLK_z:
            inputType = PlayerInputEvent::InputType::Zap;
            validInput = true;
            break;
            
        case SDLK_LCTRL:
        case SDLK_RCTRL:
        case SDLK_x:
            inputType = PlayerInputEvent::InputType::Fire;
            validInput = true;
            break;
            
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
        case SDLK_c:
            inputType = PlayerInputEvent::InputType::SuperZapper;
            validInput = true;
            break;
            
        default:
            break;
    }
#endif
    
    if (validInput) {
        PlayerInputEvent inputEvent(inputType, false);
        EventSystem::getInstance().dispatch(inputEvent);
        spdlog::debug("Key released: {}, Input type: {}", key, static_cast<int>(inputType));
    }
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

// GameEngineSingleton implementation
GameEngine& GameEngineSingleton::getInstance() {
    spdlog::info(">>> Entering GameEngineSingleton::getInstance()");
    static GameEngine instance;
    spdlog::info(">>> Exiting GameEngineSingleton::getInstance()");
    return instance;
}

} // namespace Tempest 