#include <iostream>
#include <SDL.h>
#include <spdlog/spdlog.h>
#include "Core/GameEngine.h"

int main(int argc, char* argv[]) {
    // Initialize logging
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting Tempest Rebuild Project...");
    
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        spdlog::error("SDL2 initialization failed: {}", SDL_GetError());
        return 1;
    }
    
    spdlog::info("SDL2 initialized successfully");
    
    // Initialize the game engine
    auto& engine = Tempest::GameEngineSingleton::getInstance();
    
    if (!engine.initialize()) {
        spdlog::error("Failed to initialize game engine");
        SDL_Quit();
        return 1;
    }
    
    spdlog::info("Game engine initialized successfully");
    
    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "Tempest Rebuild",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        600,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
    );
    
    if (!window) {
        spdlog::error("Failed to create window: {}", SDL_GetError());
        engine.shutdown();
        SDL_Quit();
        return 1;
    }
    
    spdlog::info("Window created successfully");
    
    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    if (!renderer) {
        spdlog::error("Failed to create renderer: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        engine.shutdown();
        SDL_Quit();
        return 1;
    }
    
    spdlog::info("Renderer created successfully");
    
    // Set up engine configuration
    engine.setTargetFPS(60.0f);
    engine.setMaxFrameTime(0.033f); // 30 FPS minimum
    
    // Create some test entities to verify ECS system
    auto& entityManager = engine.getEntityManager();
    
    // Create a test entity
    auto testEntityId = entityManager.createEntity();
    spdlog::info("Created test entity with ID: {}", testEntityId);
    
    // Main game loop
    spdlog::info("Starting main game loop...");
    engine.run();
    
    // Cleanup
    spdlog::info("Shutting down...");
    engine.shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    spdlog::info("Tempest Rebuild Project terminated successfully");
    return 0;
} 