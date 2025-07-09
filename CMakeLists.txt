cmake_minimum_required(VERSION 3.20)
project(TempestRebuild VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set UTF-8 support for Windows
if(MSVC)
    add_compile_options(/utf-8)
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(SDL2 CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/Core)
include_directories(${CMAKE_SOURCE_DIR}/src/Game)
include_directories(${CMAKE_SOURCE_DIR}/src/Graphics)
include_directories(${CMAKE_SOURCE_DIR}/src/Audio)
include_directories(${CMAKE_SOURCE_DIR}/src/Input)
include_directories(${CMAKE_SOURCE_DIR}/src/Platform)

# Add stb headers (header-only library)
include_directories(${CMAKE_SOURCE_DIR}/src/Graphics)

# Create main executable
add_executable(TempestRebuild
    src/main.cpp
    src/Core/GameEngine.cpp
    src/Core/EntityManager.cpp
    src/Core/EventSystem.cpp
    src/Core/GameState.cpp
    src/Game/TempestGame.cpp
    src/Game/Player.cpp
    src/Game/TubeGeometry.cpp
    src/Game/Enemy.cpp
    src/Game/Projectile.cpp
    src/Game/Collision.cpp
    src/Game/Level.cpp
    src/Game/LevelManager.cpp
    src/Game/ScoreManager.cpp
    src/Graphics/Renderer.cpp
    src/Graphics/Shader.cpp
    src/Graphics/Texture.cpp
    src/Graphics/Sprite.cpp
    src/Audio/AudioManager.cpp
    src/Audio/SoundEffect.cpp
    src/Audio/Music.cpp
    src/Input/InputManager.cpp
    src/Input/Keyboard.cpp
    src/Input/Mouse.cpp
    src/Platform/PlatformManager.cpp
    src/Platform/Windows/WindowsPlatform.cpp
)

# Link libraries
target_link_libraries(TempestRebuild
    PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    OpenAL::OpenAL
    glfw
    glm::glm
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)

# Set compiler-specific flags
if(MSVC)
    target_compile_options(TempestRebuild PRIVATE /W4)
else()
    target_compile_options(TempestRebuild PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Enable testing
enable_testing()
find_package(GTest CONFIG REQUIRED)

# Add test executable
add_executable(TempestTests
    tests/unit/TestGameEngine.cpp
    tests/unit/TestRenderer.cpp
    tests/unit/TestAudioManager.cpp
    tests/unit/TestInputManager.cpp
    tests/unit/TestPlayer.cpp
    tests/unit/TestEnemy.cpp
    tests/unit/TestProjectile.cpp
    tests/unit/TestLevelManager.cpp
    tests/unit/TestScoreManager.cpp
    src/Core/GameEngine.cpp
    src/Core/EntityManager.cpp
    src/Core/EventSystem.cpp
    src/Core/GameState.cpp
    src/Game/TempestGame.cpp
    src/Game/Player.cpp
    src/Game/TubeGeometry.cpp
    src/Game/Enemy.cpp
    src/Game/Projectile.cpp
    src/Game/Collision.cpp
    src/Game/Level.cpp
    src/Game/LevelManager.cpp
    src/Game/ScoreManager.cpp
)

target_link_libraries(TempestTests
    PRIVATE
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    spdlog::spdlog
)

# Add tests
add_test(NAME TempestTests COMMAND TempestTests)

# Install rules
install(TARGETS TempestRebuild
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY assets/
    DESTINATION bin/assets
    FILES_MATCHING PATTERN "*.png" PATTERN "*.jpg" PATTERN "*.wav" PATTERN "*.mp3" PATTERN "*.ttf"
) 