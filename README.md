# Tempest Arcade Game Recreation

## Project Status (July 8, 2025)

- Core engine, ECS, and event system: **Complete**
- Player system, tube geometry, and movement: **Complete**
- Enemy system (all types, AI, spawning): **Complete**
- Weapon, projectile, and collision systems: **Complete**
- All core gameplay systems validated by comprehensive tests
- Next: Game state management, level system, advanced rendering

## Overview
This project is a faithful recreation of the original Tempest arcade game, following the original assembly and memory layout. The codebase is modular, test-driven, and designed for extensibility.

## Features
- Entity Component System (ECS) architecture
- Event-driven game loop
- Player movement and controls
- 3D tube geometry and collision
- Enemy AI and spawning (Flipper, Pulsar, Tanker, Spiker, Fuzzball)
- Weapon, projectile, and collision systems
- Comprehensive unit and integration tests

## Next Steps
- Implement game state management and level progression
- Add advanced vector graphics and rendering
- Polish gameplay, effects, and UI

## How to Build and Test

1. **Prerequisites**
   - C++20 compatible compiler
   - CMake 3.20+
   - Git
   - Platform-specific development tools

2. **Build Instructions**
   ```bash
   git clone https://github.com/your-org/tempest-rebuild.git
   cd tempest-rebuild
   mkdir build && cd build
   cmake ..
   make
   ```

3. **Running the Game**
   ```bash
   ./tempest-rebuild
   ```

## Contributing

Please read [CONTRIBUTING.md](docs/contributing.md) for details on our code of conduct and the process for submitting pull requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Original Tempest development team at Atari
- The reverse engineering community for the disassembly
- Open source projects that made this rebuild possible

---

*This project is a tribute to the original Tempest arcade game and is not affiliated with Atari.* 