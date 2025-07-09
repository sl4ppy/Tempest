# Cursor IDE Setup Guide

This guide covers setting up and using Cursor IDE for the Tempest rebuild project.

## What is Cursor?

Cursor is an AI-powered code editor built on top of VS Code that provides:
- Advanced AI code completion and generation
- Built-in chat interface for coding assistance
- Full C++ language support
- CMake integration
- Debugging capabilities
- Git integration

## Installation

### Automatic Installation
The setup script will automatically download and install Cursor from https://cursor.sh

### Manual Installation
If automatic installation fails:
1. Visit https://cursor.sh
2. Download the Windows installer
3. Run the installer and follow the prompts
4. Launch Cursor

## Initial Setup

### 1. Open the Project
```bash
# From command line
cursor .

# OR from Cursor
File → Open Folder → Select the Tempest project folder
```

### 2. Install Required Extensions
Open the Extensions panel (Ctrl+Shift+X) and install:

#### Essential Extensions
- **C/C++** (ms-vscode.cpptools)
  - Provides C++ language support, IntelliSense, debugging
  - Version: Latest stable

- **CMake Tools** (ms-vscode.cmake-tools)
  - CMake project management and build integration
  - Version: Latest stable

- **CMake** (twxs.cmake)
  - CMake language support and syntax highlighting
  - Version: Latest stable

#### Recommended Extensions
- **Clang-Format** (xaver.clang-format)
  - Code formatting using clang-format
  - Version: Latest stable

- **C++ TestMate** (matepek.vscode-catch2-test-adapter)
  - Unit test integration (optional)
  - Version: Latest stable

- **GitLens** (eamodio.gitlens)
  - Enhanced Git integration
  - Version: Latest stable

- **Error Lens** (usernamehw.errorlens)
  - Inline error and warning display
  - Version: Latest stable

### 3. Configure Cursor Settings

The setup script creates optimal settings, but you can customize:

#### Key Settings
```json
{
    "cmake.configureOnOpen": true,
    "cmake.buildDirectory": "${workspaceFolder}/build/${buildType}",
    "cmake.generator": "Visual Studio 17 2022",
    "editor.formatOnSave": true,
    "editor.tabSize": 4,
    "files.eol": "\n",
    "C_Cpp.default.cppStandard": "c++20"
}
```

## Project Structure

The Cursor workspace is configured with:

```
Tempest/
├── .vscode/                 # Cursor workspace settings
│   ├── c_cpp_properties.json  # C++ configuration
│   ├── settings.json          # Workspace settings
│   ├── tasks.json             # Build tasks
│   └── launch.json            # Debug configurations
├── src/                     # Source code
├── docs/                    # Documentation
├── build/                   # Build output
└── assets/                  # Game assets
```

## Building the Project

### Method 1: Using Cursor's CMake Integration

1. **Configure the project**:
   - Press `Ctrl+Shift+P`
   - Type "CMake: Configure"
   - Select "CMake: Configure"

2. **Build the project**:
   - Press `Ctrl+Shift+P`
   - Type "CMake: Build"
   - Select "CMake: Build" or "CMake: Build Target"

3. **Select build target**:
   - Press `Ctrl+Shift+P`
   - Type "CMake: Select a Kit"
   - Choose "Visual Studio Community 2022 Release - amd64"

### Method 2: Using Command Palette Tasks

1. **Open Command Palette**: `Ctrl+Shift+P`
2. **Select Task**: "Tasks: Run Task"
3. **Choose from available tasks**:
   - "CMake Configure"
   - "CMake Build"
   - "CMake Build Release"
   - "Run Tests"
   - "Clean Build"

### Method 3: Using Terminal

```bash
# Configure
cmake -B build -S .

# Build Debug
cmake --build build --config Debug

# Build Release
cmake --build build --config Release

# Run tests
ctest --test-dir build --output-on-failure
```

## Debugging

### Pre-configured Debug Configurations

The setup creates two debug configurations:

1. **Debug Tempest**
   - Debugs the Debug build
   - Breakpoint support
   - Variable inspection
   - Call stack navigation

2. **Release Tempest**
   - Debugs the Release build
   - Limited debugging info
   - Better performance

### Starting Debug Session

1. **Set breakpoints**: Click in the gutter next to line numbers
2. **Start debugging**: Press `F5` or use Run → Start Debugging
3. **Select configuration**: Choose "Debug Tempest" or "Release Tempest"

### Debug Controls

- **F5**: Continue
- **F10**: Step Over
- **F11**: Step Into
- **Shift+F11**: Step Out
- **F9**: Toggle Breakpoint
- **Ctrl+Shift+F5**: Restart
- **Shift+F5**: Stop

## AI Features

### Chat Interface

1. **Open Chat**: `Ctrl+L` or View → Command Palette → "Chat: Open"
2. **Ask questions** about your code, architecture, or implementation
3. **Request code generation** for specific features
4. **Get explanations** of complex code sections

### Code Completion

- **Automatic suggestions** as you type
- **Function signatures** and parameter hints
- **IntelliSense** for C++ standard library and project code
- **Context-aware completions** based on your codebase

### Code Generation

- **Generate functions** from comments
- **Create test cases** for your code
- **Implement interfaces** and classes
- **Refactor code** with AI assistance

## Keyboard Shortcuts

### Essential Shortcuts
- `Ctrl+Shift+P`: Command Palette
- `Ctrl+P`: Quick Open
- `Ctrl+Shift+E`: Explorer
- `Ctrl+Shift+X`: Extensions
- `Ctrl+Shift+G`: Source Control
- `Ctrl+Shift+D`: Debug
- `Ctrl+Shift+M`: Problems

### C++ Development
- `F12`: Go to Definition
- `Alt+F12`: Peek Definition
- `Shift+F12`: Find All References
- `Ctrl+Space`: Trigger Suggestions
- `Ctrl+Shift+Space`: Trigger Parameter Hints

### CMake Integration
- `Ctrl+Shift+P` → "CMake: Configure"
- `Ctrl+Shift+P` → "CMake: Build"
- `Ctrl+Shift+P` → "CMake: Select a Kit"

## Troubleshooting

### Common Issues

#### 1. CMake Not Found
**Problem**: CMake configuration fails
**Solution**:
- Ensure CMake is installed and in PATH
- Check that Visual Studio is properly installed
- Verify vcpkg integration

#### 2. IntelliSense Not Working
**Problem**: No code completion or error detection
**Solution**:
- Reload Cursor window (`Ctrl+Shift+P` → "Developer: Reload Window")
- Check C++ extension is installed and enabled
- Verify `c_cpp_properties.json` configuration

#### 3. Build Failures
**Problem**: CMake build errors
**Solution**:
- Check that all dependencies are installed via vcpkg
- Verify Visual Studio C++ tools are installed
- Check build output for specific error messages

#### 4. Debugger Not Working
**Problem**: Can't start debugging session
**Solution**:
- Ensure the project is built successfully
- Check that the executable exists at the expected path
- Verify launch.json configuration

### Getting Help

1. **Check Cursor documentation**: https://cursor.sh/docs
2. **Review project documentation**: `docs/` folder
3. **Use Cursor's built-in help**: `Ctrl+Shift+P` → "Help: Show All Commands"
4. **Check extension documentation**: Each extension has its own help

## Best Practices

### Code Organization
- Use the established project structure
- Follow the coding standards in `docs/development.md`
- Keep related files together in appropriate directories

### Development Workflow
1. **Plan your changes** using the implementation plan
2. **Create feature branches** for new development
3. **Write tests** for new functionality
4. **Use Cursor's AI** for code generation and assistance
5. **Debug thoroughly** before committing

### Performance Tips
- Use Release builds for performance testing
- Enable compiler optimizations in CMake
- Use Cursor's built-in profiling tools
- Monitor memory usage during development

## Integration with Project Tools

### Git Integration
- **Source Control panel**: View changes and commit
- **GitLens extension**: Enhanced Git features
- **Branch management**: Switch between branches easily

### Testing
- **C++ TestMate**: Run and debug unit tests
- **Test Explorer**: View test results and coverage
- **Debug tests**: Set breakpoints in test code

### Documentation
- **Markdown preview**: View documentation files
- **Code documentation**: Generate and view Doxygen docs
- **API reference**: Quick access to function documentation

---

**Happy coding with Cursor! 🎮** 