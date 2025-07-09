# Tempest Rebuild Project - Development Environment Setup

This guide will help you set up a complete development environment for the Tempest rebuild project on Windows 11.

## Prerequisites

Before running the setup script, ensure you have:

- **Windows 11** (or Windows 10 with latest updates)
- **Internet connection** (for downloading tools and libraries)
- **Administrator privileges** (recommended for best results)
- **At least 10GB free disk space**
- **PowerShell 5.1 or later** (included with Windows 10/11)

## Quick Setup

### Option 1: Automated Setup (Recommended)

1. **Download the project** (if you haven't already):
   ```bash
   git clone <repository-url>
   cd Tempest
   ```

2. **Run the setup script**:
   ```bash
   # Double-click setup-dev.bat
   # OR run from command line:
   setup-dev.bat
   ```

3. **Wait for completion**:
   The script will automatically install all required tools and configure your environment. This typically takes 30-60 minutes.

### Option 2: Manual Setup

If you prefer to install components manually or the automated setup fails:

#### 1. Install Chocolatey Package Manager
```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

#### 2. Install Core Tools
```powershell
# Git
choco install git -y

# Visual Studio 2022 Community
# Download from: https://visualstudio.microsoft.com/downloads/
# Install with: Native Desktop Development, CMake Tools, Windows 10 SDK

# CMake
choco install cmake -y

# Cursor IDE
# Download from: https://cursor.sh
# The setup script will attempt to download and install automatically
```

#### 3. Install vcpkg and Libraries
```powershell
# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git tools\vcpkg
cd tools\vcpkg
.\bootstrap-vcpkg.bat

# Install required libraries
.\vcpkg install sdl2:x64-windows
.\vcpkg install openal-soft:x64-windows
.\vcpkg install glfw3:x64-windows
.\vcpkg install glm:x64-windows
.\vcpkg install stb:x64-windows
.\vcpkg install nlohmann-json:x64-windows
.\vcpkg install spdlog:x64-windows
.\vcpkg install gtest:x64-windows

# Integrate with Visual Studio
.\vcpkg integrate install
```

#### 4. Configure Cursor IDE Extensions
After installing Cursor, open the Extensions marketplace (Ctrl+Shift+X) and install:
- **C/C++** (ms-vscode.cpptools) - C++ language support
- **CMake Tools** (ms-vscode.cmake-tools) - CMake integration
- **CMake** (twxs.cmake) - CMake language support
- **Clang-Format** (xaver.clang-format) - Code formatting
- **C++ TestMate** (matepek.vscode-catch2-test-adapter) - Unit testing (optional)

## What Gets Installed

The setup script installs the following components:

### Core Development Tools
- **Git** - Version control
- **Visual Studio 2022 Community** - C++ compiler and build tools
- **CMake** - Build system generator
- **Cursor IDE** - AI-powered code editor with C++ support

### Package Managers
- **Chocolatey** - Windows package manager
- **vcpkg** - C++ library manager

### Libraries and Dependencies
- **SDL2** - Cross-platform multimedia library
- **OpenAL** - 3D audio library
- **GLFW** - OpenGL framework
- **GLM** - Mathematics library
- **stb** - Single-file public domain libraries
- **nlohmann-json** - JSON library
- **spdlog** - Fast logging library
- **Google Test** - Unit testing framework

### Project Configuration
- Complete project directory structure
- CMake configuration files
- Cursor IDE workspace settings (including debug configurations)
- Git repository initialization
- Build scripts

## Verification

After setup, verify your environment:

```bash
# Test core tools
git --version
cmake --version
cl
vcpkg version

# Test build system
.\build.bat

# Test project structure
dir src
dir docs
dir scripts
```

## Troubleshooting

### Common Issues

#### 1. PowerShell Execution Policy
**Error**: "Execution policy prevents running scripts"

**Solution**:
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

#### 2. Visual Studio Not Found
**Error**: "Visual Studio not found" or "cl.exe not found"

**Solution**:
- Install Visual Studio 2022 Community
- Ensure "Desktop development with C++" workload is installed
- Run setup from Developer Command Prompt

#### 3. vcpkg Integration Issues
**Error**: "Package not found" or linking errors

**Solution**:
```powershell
cd tools\vcpkg
.\vcpkg integrate install
.\vcpkg integrate remove
.\vcpkg integrate install
```

#### 4. CMake Configuration Errors
**Error**: "CMake configuration failed"

**Solution**:
- Ensure vcpkg is properly installed
- Check that all libraries are installed
- Verify Visual Studio installation

#### 5. Build Failures
**Error**: "Build failed" or compilation errors

**Solution**:
- Check that all dependencies are installed
- Verify CMake configuration
- Check for missing include paths

### Getting Help

If you encounter issues not covered here:

1. **Check the logs**: Look for error messages in the setup output
2. **Verify prerequisites**: Ensure all system requirements are met
3. **Try manual installation**: Use the manual setup steps above
4. **Check documentation**: Review the project documentation in `docs/`
5. **Report issues**: Create an issue in the project repository

## Next Steps

After successful setup:

1. **Open the project**:
   ```bash
   cursor .
   ```
   OR double-click Cursor and open the project folder

2. **Build the project**:
   ```bash
   .\build.bat
   ```
   OR use Cursor's built-in CMake integration (Ctrl+Shift+P → "CMake: Configure")

3. **Run the project**:
   ```bash
   .\run.bat
   ```
   OR use Cursor's debugger (F5) with the pre-configured debug configurations

4. **Read the documentation**:
   - `docs/README.md` - Project overview
   - `docs/architecture.md` - System architecture
   - `docs/implementation-plan.md` - Development roadmap

5. **Start development**:
   Follow the implementation plan in `docs/implementation-plan.md`

## Environment Variables

The setup script configures these environment variables:

- `VCPKG_ROOT` - Points to the vcpkg installation
- `PATH` - Updated to include vcpkg tools

## Customization

You can customize the setup by modifying the PowerShell script:

- **Skip components**: Use command-line parameters like `-SkipCursor`
- **Change installation paths**: Modify the configuration variables
- **Add additional libraries**: Extend the `Install-Libraries` function
- **Modify build configuration**: Edit the generated CMake files

## Support

For additional support:

- Check the project documentation in the `docs/` folder
- Review the implementation plan for development guidance
- Consult the architecture documentation for system design
- Refer to the API reference for detailed interfaces

---

**Happy coding! 🎮** 