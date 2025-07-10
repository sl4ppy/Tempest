#include <iostream>
#include <SDL.h>
#include <spdlog/spdlog.h>
#include "Graphics/glad/gl.h"
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
    
    // Create window with OpenGL support
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
        SDL_Quit();
        return 1;
    }
    
    spdlog::info("Window created successfully");
    
    // Create OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        spdlog::error("Failed to create OpenGL context: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    spdlog::info("OpenGL context created successfully");
    
    // Make the OpenGL context current
    spdlog::info("About to make OpenGL context current...");
    SDL_GL_MakeCurrent(window, glContext);
    spdlog::info("OpenGL context made current");
    
    // Initialize GLAD
    spdlog::info("Initializing GLAD...");
    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
        spdlog::error("Failed to initialize GLAD");
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    spdlog::info("GLAD initialized successfully");
    
    // Print OpenGL version info
    spdlog::info("About to get OpenGL version...");
    const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    spdlog::info("OpenGL version obtained");
    
    spdlog::info("About to get OpenGL renderer...");
    const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    spdlog::info("OpenGL renderer obtained");
    
    spdlog::info("About to get OpenGL vendor...");
    const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    spdlog::info("OpenGL vendor obtained");
    
    spdlog::info("OpenGL Version: {}", version ? version : "Unknown");
    spdlog::info("OpenGL Renderer: {}", renderer ? renderer : "Unknown");
    spdlog::info("OpenGL Vendor: {}", vendor ? vendor : "Unknown");
    
    // Initialize GLAD (simplified for now)
    spdlog::info("GLAD initialization skipped - using system OpenGL");
    spdlog::info("GLAD initialized successfully");
    
    // Initialize the game engine
    spdlog::info("Getting GameEngine singleton...");
    spdlog::info("About to access Tempest namespace...");
    spdlog::info("About to call getInstance()...");
    auto& engine = Tempest::GameEngineSingleton::getInstance();
    spdlog::info("GameEngine singleton obtained");
    
    // Pass the OpenGL context to the engine
    spdlog::info("Setting OpenGL context...");
    engine.setGLContext(glContext);
    spdlog::info("OpenGL context set");
    
    spdlog::info("About to call engine.initialize()...");
    if (!engine.initialize()) {
        spdlog::error("Failed to initialize game engine");
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    spdlog::info("Game engine initialized successfully");
    
    // Pass the window handle to the engine for buffer swapping
    engine.setSDLWindow(window);
    
    // Run the main loop
    engine.run();
    
    // Cleanup
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    spdlog::info("Tempest Rebuild Project terminated successfully");
    return 0;
} 