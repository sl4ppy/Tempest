# Tempest Rebuild Project - Windows 11 Development Environment Setup
# This script sets up a complete development environment for the Tempest rebuild project
# Run this script as Administrator for best results

param(
    [switch]$SkipChocolatey,
    [switch]$SkipCursor,
    [switch]$SkipGit,
    [switch]$SkipCMake,
    [switch]$SkipVisualStudio,
    [switch]$SkipLibraries,
    [switch]$SkipConfig,
    [switch]$Force
)

# Set execution policy to allow script execution
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser -Force

# Configuration
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$ToolsDir = Join-Path $ProjectRoot "tools"
$BuildDir = Join-Path $ProjectRoot "build"
$VcpkgDir = Join-Path $ToolsDir "vcpkg"

# Colors for output
$Red = "Red"
$Green = "Green"
$Yellow = "Yellow"
$Blue = "Blue"

function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$Color = "White"
    )
    Write-Host $Message -ForegroundColor $Color
}

function Test-Command {
    param([string]$Command)
    try {
        Get-Command $Command -ErrorAction Stop | Out-Null
        return $true
    }
    catch {
        return $false
    }
}

function Test-Admin {
    $currentUser = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($currentUser)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

function Install-Chocolatey {
    Write-ColorOutput "Installing Chocolatey package manager..." $Blue
    
    if (Test-Command "choco") {
        Write-ColorOutput "Chocolatey is already installed." $Green
        return
    }
    
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    
    # Refresh environment variables
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
    
    Write-ColorOutput "Chocolatey installed successfully." $Green
}

function Install-Git {
    Write-ColorOutput "Installing Git..." $Blue
    
    if (Test-Command "git") {
        Write-ColorOutput "Git is already installed." $Green
        return
    }
    
    choco install git -y
    
    # Configure Git
    git config --global core.autocrlf true
    git config --global core.eol lf
    git config --global init.defaultBranch main
    
    Write-ColorOutput "Git installed and configured successfully." $Green
}

function Install-VisualStudio {
    Write-ColorOutput "Installing Visual Studio 2022 Community..." $Blue
    
    # Check if Visual Studio is already installed
    $vsInstallPath = "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community"
    if (Test-Path $vsInstallPath) {
        Write-ColorOutput "Visual Studio 2022 Community is already installed." $Green
        return
    }
    
    # Download Visual Studio Installer
    $vsInstallerUrl = "https://aka.ms/vs/17/release/vs_community.exe"
    $vsInstallerPath = Join-Path $env:TEMP "vs_community.exe"
    
    Write-ColorOutput "Downloading Visual Studio Installer..." $Yellow
    Invoke-WebRequest -Uri $vsInstallerUrl -OutFile $vsInstallerPath
    
    # Install Visual Studio with required workloads
    Write-ColorOutput "Installing Visual Studio (this may take a while)..." $Yellow
    Start-Process -FilePath $vsInstallerPath -ArgumentList @(
        "--quiet",
        "--wait",
        "--norestart",
        "--add Microsoft.VisualStudio.Workload.NativeDesktop",
        "--add Microsoft.VisualStudio.Workload.NativeCrossPlat",
        "--add Microsoft.VisualStudio.Component.CMake.Project",
        "--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
        "--add Microsoft.VisualStudio.Component.Windows10SDK.19041",
        "--add Microsoft.VisualStudio.Component.Git.GitCredentialManager"
    ) -Wait
    
    # Clean up installer
    Remove-Item $vsInstallerPath -Force
    
    Write-ColorOutput "Visual Studio 2022 Community installed successfully." $Green
}

function Install-CMake {
    Write-ColorOutput "Installing CMake..." $Blue
    
    if (Test-Command "cmake") {
        Write-ColorOutput "CMake is already installed." $Green
        return
    }
    
    choco install cmake -y
    
    Write-ColorOutput "CMake installed successfully." $Green
}

function Install-Cursor {
    Write-ColorOutput "Installing Cursor IDE..." $Blue
    
    if (Test-Command "cursor") {
        Write-ColorOutput "Cursor is already installed." $Green
        return
    }
    
    # Download and install Cursor
    Write-ColorOutput "Downloading Cursor..." $Yellow
    $cursorUrl = "https://download.cursor.sh/windows"
    $cursorInstallerPath = Join-Path $env:TEMP "cursor-setup.exe"
    
    try {
        Invoke-WebRequest -Uri $cursorUrl -OutFile $cursorInstallerPath
        Write-ColorOutput "Installing Cursor..." $Yellow
        Start-Process -FilePath $cursorInstallerPath -ArgumentList "/S" -Wait
        Remove-Item $cursorInstallerPath -Force
        Write-ColorOutput "Cursor installed successfully." $Green
    }
    catch {
        Write-ColorOutput "Failed to download Cursor automatically. Please install manually from https://cursor.sh" $Yellow
        Write-ColorOutput "After installation, you can install extensions manually." $Yellow
    }
    
    # Note: Cursor extensions are typically installed through the IDE interface
    # rather than command line, so we'll provide instructions instead
    Write-ColorOutput "Note: Install C++ extensions through Cursor's extension marketplace after first launch." $Yellow
}

function Install-Vcpkg {
    Write-ColorOutput "Installing vcpkg package manager..." $Blue
    
    if (Test-Path $VcpkgDir) {
        Write-ColorOutput "vcpkg is already installed." $Green
        return
    }
    
    # Create tools directory
    New-Item -ItemType Directory -Path $ToolsDir -Force | Out-Null
    
    # Clone vcpkg
    git clone https://github.com/Microsoft/vcpkg.git $VcpkgDir
    
    # Bootstrap vcpkg
    Push-Location $VcpkgDir
    .\bootstrap-vcpkg.bat
    Pop-Location
    
    # Add vcpkg to PATH
    $vcpkgPath = Join-Path $VcpkgDir "installed\x64-windows\tools"
    $env:Path += ";$vcpkgPath"
    [Environment]::SetEnvironmentVariable("Path", $env:Path, "User")
    
    Write-ColorOutput "vcpkg installed successfully." $Green
}

function Install-Libraries {
    Write-ColorOutput "Installing required libraries..." $Blue
    
    Push-Location $VcpkgDir
    
    # Install core libraries
    Write-ColorOutput "Installing SDL2..." $Yellow
    .\vcpkg install sdl2:x64-windows
    
    Write-ColorOutput "Installing OpenAL..." $Yellow
    .\vcpkg install openal-soft:x64-windows
    
    Write-ColorOutput "Installing GLFW..." $Yellow
    .\vcpkg install glfw3:x64-windows
    
    Write-ColorOutput "Installing GLM..." $Yellow
    .\vcpkg install glm:x64-windows
    
    Write-ColorOutput "Installing stb..." $Yellow
    .\vcpkg install stb:x64-windows
    
    Write-ColorOutput "Installing nlohmann-json..." $Yellow
    .\vcpkg install nlohmann-json:x64-windows
    
    Write-ColorOutput "Installing spdlog..." $Yellow
    .\vcpkg install spdlog:x64-windows
    
    Write-ColorOutput "Installing Google Test..." $Yellow
    .\vcpkg install gtest:x64-windows
    
    # Integrate with Visual Studio
    .\vcpkg integrate install
    
    Pop-Location
    
    Write-ColorOutput "All libraries installed successfully." $Green
}

function Setup-ProjectStructure {
    Write-ColorOutput "Setting up project structure..." $Blue
    
    # Create necessary directories
    $directories = @(
        "src",
        "src\Core",
        "src\Game",
        "src\Graphics",
        "src\Audio",
        "src\Input",
        "src\Platform",
        "src\Platform\Windows",
        "assets",
        "assets\audio",
        "assets\graphics",
        "assets\fonts",
        "tests",
        "tests\unit",
        "tests\integration",
        "docs",
        "scripts",
        "tools",
        "build",
        "build\Debug",
        "build\Release",
        "build\RelWithDebInfo",
        "build\MinSizeRel"
    )
    
    foreach ($dir in $directories) {
        $path = Join-Path $ProjectRoot $dir
        New-Item -ItemType Directory -Path $path -Force | Out-Null
    }
    
    Write-ColorOutput "Project structure created successfully." $Green
}

function Create-CMakeLists {
    Write-ColorOutput "Creating CMake configuration files..." $Blue
    
    # Main CMakeLists.txt
    $cmakeContent = @"
cmake_minimum_required(VERSION 3.20)
project(Tempest VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(ENABLE_VULKAN "Enable Vulkan renderer" ON)
option(ENABLE_DEBUG "Enable debug features" ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY \${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY \${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY \${CMAKE_BINARY_DIR}/lib)

# Find vcpkg
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "\$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# Find packages
find_package(SDL2 REQUIRED)
find_package(OpenAL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)
    enable_testing()
endif()

# Platform detection
if(WIN32)
    set(PLATFORM_SOURCES src/Platform/Windows/WindowsPlatform.cpp)
    set(PLATFORM_LIBS)
endif()

# Graphics API selection
if(ENABLE_VULKAN AND VULKAN_FOUND)
    set(GRAPHICS_API "Vulkan")
    set(GRAPHICS_SOURCES src/Graphics/VulkanRenderer.cpp)
else()
    set(GRAPHICS_API "OpenGL")
    set(GRAPHICS_SOURCES src/Graphics/OpenGLRenderer.cpp)
endif()

# Add subdirectories
add_subdirectory(src)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
"@
    
    $cmakePath = Join-Path $ProjectRoot "CMakeLists.txt"
    $cmakeContent | Out-File -FilePath $cmakePath -Encoding UTF8
    
    # Source CMakeLists.txt
    $srcCmakeContent = @"
# Source CMakeLists.txt

# Collect source files
file(GLOB_RECURSE CORE_SOURCES "src/Core/*.cpp")
file(GLOB_RECURSE GAME_SOURCES "src/Game/*.cpp")
file(GLOB_RECURSE GRAPHICS_SOURCES "src/Graphics/*.cpp")
file(GLOB_RECURSE AUDIO_SOURCES "src/Audio/*.cpp")
file(GLOB_RECURSE INPUT_SOURCES "src/Input/*.cpp")

# Create main executable
add_executable(Tempest
    \${CORE_SOURCES}
    \${GAME_SOURCES}
    \${GRAPHICS_SOURCES}
    \${AUDIO_SOURCES}
    \${INPUT_SOURCES}
    \${PLATFORM_SOURCES}
    \${GRAPHICS_SOURCES}
)

# Set target properties
set_target_properties(Tempest PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Include directories
target_include_directories(Tempest PRIVATE
    src
    \${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(Tempest PRIVATE
    SDL2::SDL2
    OpenAL::AL
    glfw
    glm::glm
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    \${PLATFORM_LIBS}
)

# Compiler-specific settings
if(MSVC)
    target_compile_options(Tempest PRIVATE /W4 /WX)
else()
    target_compile_options(Tempest PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Copy assets
add_custom_command(TARGET Tempest POST_BUILD
    COMMAND \${CMAKE_COMMAND} -E copy_directory
    \${CMAKE_SOURCE_DIR}/assets
    \${CMAKE_BINARY_DIR}/bin/assets
)
"@
    
    $srcCmakePath = Join-Path $ProjectRoot "src\CMakeLists.txt"
    $srcCmakeContent | Out-File -FilePath $srcCmakePath -Encoding UTF8
    
    Write-ColorOutput "CMake configuration files created successfully." $Green
}

function Create-CursorConfig {
    Write-ColorOutput "Creating Cursor IDE configuration..." $Blue
    
    # Create .vscode directory (Cursor uses the same format as VS Code)
    $cursorDir = Join-Path $ProjectRoot ".vscode"
    New-Item -ItemType Directory -Path $cursorDir -Force | Out-Null
    
    # c_cpp_properties.json
    $cppProperties = @{
        configurations = @(
            @{
                name = "Win32"
                includePath = @(
                    "`${workspaceFolder}/src",
                    "`${workspaceFolder}/tools/vcpkg/installed/x64-windows/include"
                )
                defines = @(
                    "_DEBUG",
                    "UNICODE",
                    "_UNICODE"
                )
                windowsSdkVersion = "10.0.19041.0"
                compilerPath = "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64/cl.exe"
                cStandard = "c17"
                cppStandard = "c++20"
                intelliSenseMode = "windows-msvc-x64"
                configurationProvider = "ms-vscode.cmake-tools"
            }
        )
        version = 4
    }
    
    $cppPropertiesPath = Join-Path $cursorDir "c_cpp_properties.json"
    $cppProperties | ConvertTo-Json -Depth 10 | Out-File -FilePath $cppPropertiesPath -Encoding UTF8
    
    # settings.json
    $settings = @{
        "cmake.configureOnOpen" = $true
        "cmake.buildDirectory" = "`${workspaceFolder}/build/`${buildType}"
        "cmake.generator" = "Visual Studio 17 2022"
        "cmake.preferredGenerators" = @("Visual Studio 17 2022")
        "files.associations" = @{
            "*.h" = "c"
            "*.hpp" = "cpp"
            "*.cpp" = "cpp"
            "*.c" = "c"
        }
        "C_Cpp.default.configurationProvider" = "ms-vscode.cmake-tools"
        "editor.formatOnSave" = $true
        "editor.formatOnType" = $true
        "files.trimTrailingWhitespace" = $true
        "files.insertFinalNewline" = $true
        "editor.tabSize" = 4
        "editor.insertSpaces" = $true
        "editor.detectIndentation" = $false
        "files.eol" = "\n"
        "C_Cpp.default.cStandard" = "c17"
        "C_Cpp.default.cppStandard" = "c++20"
        "C_Cpp.default.intelliSenseMode" = "windows-msvc-x64"
    }
    
    $settingsPath = Join-Path $cursorDir "settings.json"
    $settings | ConvertTo-Json -Depth 10 | Out-File -FilePath $settingsPath -Encoding UTF8
    
    # tasks.json
    $tasks = @{
        version = "2.0.0"
        tasks = @(
            @{
                label = "CMake Configure"
                type = "shell"
                command = "cmake"
                args = @("-B", "build", "-S", ".")
                group = "build"
                presentation = @{
                    echo = $true
                    reveal = "always"
                    focus = $false
                    panel = "shared"
                    showReuseMessage = $true
                    clear = $false
                }
            },
            @{
                label = "CMake Build"
                type = "shell"
                command = "cmake"
                args = @("--build", "build", "--config", "Debug")
                group = @{
                    kind = "build"
                    isDefault = $true
                }
                dependsOn = "CMake Configure"
                presentation = @{
                    echo = $true
                    reveal = "always"
                    focus = $false
                    panel = "shared"
                    showReuseMessage = $true
                    clear = $false
                }
            },
            @{
                label = "CMake Build Release"
                type = "shell"
                command = "cmake"
                args = @("--build", "build", "--config", "Release")
                group = "build"
                presentation = @{
                    echo = $true
                    reveal = "always"
                    focus = $false
                    panel = "shared"
                    showReuseMessage = $true
                    clear = $false
                }
            },
            @{
                label = "Run Tests"
                type = "shell"
                command = "ctest"
                args = @("--test-dir", "build", "--output-on-failure")
                group = "test"
                dependsOn = "CMake Build"
            },
            @{
                label = "Clean Build"
                type = "shell"
                command = "cmake"
                args = @("--build", "build", "--target", "clean")
                group = "build"
                presentation = @{
                    echo = $true
                    reveal = "always"
                    focus = $false
                    panel = "shared"
                    showReuseMessage = $true
                    clear = $false
                }
            }
        )
    }
    
    $tasksPath = Join-Path $cursorDir "tasks.json"
    $tasks | ConvertTo-Json -Depth 10 | Out-File -FilePath $tasksPath -Encoding UTF8
    
    # launch.json for debugging
    $launch = @{
        version = "0.2.0"
        configurations = @(
            @{
                name = "Debug Tempest"
                type = "cppvsdbg"
                request = "launch"
                program = "`${workspaceFolder}/build/bin/Debug/Tempest.exe"
                args = @()
                stopAtEntry = $false
                cwd = "`${workspaceFolder}/build/bin/Debug"
                environment = @()
                externalConsole = $false
                MIMode = "gdb"
                setupCommands = @(
                    @{
                        description = "Enable pretty-printing for gdb"
                        text = "-enable-pretty-printing"
                        ignoreFailures = $true
                    }
                )
            },
            @{
                name = "Release Tempest"
                type = "cppvsdbg"
                request = "launch"
                program = "`${workspaceFolder}/build/bin/Release/Tempest.exe"
                args = @()
                stopAtEntry = $false
                cwd = "`${workspaceFolder}/build/bin/Release"
                environment = @()
                externalConsole = $false
                MIMode = "gdb"
            }
        )
    }
    
    $launchPath = Join-Path $cursorDir "launch.json"
    $launch | ConvertTo-Json -Depth 10 | Out-File -FilePath $launchPath -Encoding UTF8
    
    Write-ColorOutput "Cursor IDE configuration created successfully." $Green
}

function Create-GitConfig {
    Write-ColorOutput "Creating Git configuration..." $Blue
    
    # .gitignore
    $gitignoreContent = @"
# Build directories
build/
out/
Debug/
Release/
RelWithDebInfo/
MinSizeRel/

# IDE files
.vs/
.vscode/settings.json
.vscode/launch.json
.vscode/tasks.json
*.vcxproj.user
*.vcxproj.filters

# CMake files
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile
*.cmake

# Compiled files
*.o
*.obj
*.exe
*.dll
*.so
*.dylib
*.a
*.lib

# Logs
*.log
logs/

# Temporary files
*.tmp
*.temp
*~

# OS generated files
.DS_Store
.DS_Store?
._*
.Spotlight-V100
.Trashes
ehthumbs.db
Thumbs.db

# Package files
*.zip
*.tar.gz
*.7z

# Asset files (uncomment if you don't want to track assets)
# assets/
"@
    
    $gitignorePath = Join-Path $ProjectRoot ".gitignore"
    $gitignoreContent | Out-File -FilePath $gitignorePath -Encoding UTF8
    
    # Initialize Git repository if not already done
    if (-not (Test-Path (Join-Path $ProjectRoot ".git"))) {
        git init
        git add .
        git commit -m "Initial commit: Project setup"
    }
    
    Write-ColorOutput "Git configuration created successfully." $Green
}

function Create-BuildScript {
    Write-ColorOutput "Creating build scripts..." $Blue
    
    # build.bat
    $buildBatContent = @"
@echo off
setlocal enabledelayedexpansion

echo Building Tempest Rebuild Project...

REM Set vcpkg root if not set
if not defined VCPKG_ROOT (
    set VCPKG_ROOT=%~dp0tools\vcpkg
)

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake

if errorlevel 1 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build the project
echo Building project...
cmake --build . --config Debug

if errorlevel 1 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build completed successfully!
echo Executable location: build\bin\Debug\Tempest.exe

cd ..
pause
"@
    
    $buildBatPath = Join-Path $ProjectRoot "build.bat"
    $buildBatContent | Out-File -FilePath $buildBatPath -Encoding ASCII
    
    # run.bat
    $runBatContent = @"
@echo off
echo Running Tempest Rebuild Project...

if exist build\bin\Debug\Tempest.exe (
    cd build\bin\Debug
    Tempest.exe
    cd ..\..\..
) else (
    echo Executable not found. Please build the project first.
    echo Run build.bat to build the project.
    pause
)
"@
    
    $runBatPath = Join-Path $ProjectRoot "run.bat"
    $runBatContent | Out-File -FilePath $runBatPath -Encoding ASCII
    
    Write-ColorOutput "Build scripts created successfully." $Green
}

function Test-Environment {
    Write-ColorOutput "Testing development environment..." $Blue
    
    $tests = @(
        @{ Name = "Git"; Command = "git --version" },
        @{ Name = "CMake"; Command = "cmake --version" },
        @{ Name = "Visual Studio"; Command = "cl" },
        @{ Name = "vcpkg"; Command = "vcpkg version" }
    )
    
    $allPassed = $true
    
    foreach ($test in $tests) {
        try {
            $result = Invoke-Expression $test.Command 2>$null
            if ($LASTEXITCODE -eq 0) {
                Write-ColorOutput "âœ“ $($test.Name): OK" $Green
            } else {
                Write-ColorOutput "âœ— $($test.Name): Failed" $Red
                $allPassed = $false
            }
        }
        catch {
            Write-ColorOutput "âœ— $($test.Name): Failed" $Red
            $allPassed = $false
        }
    }
    
    if ($allPassed) {
        Write-ColorOutput "All environment tests passed!" $Green
    } else {
        Write-ColorOutput "Some environment tests failed. Please check the installation." $Red
    }
    
    return $allPassed
}

function Show-NextSteps {
    Write-ColorOutput "`n=== Development Environment Setup Complete ===" $Green
    Write-ColorOutput "`nNext steps:" $Blue
    Write-ColorOutput "1. Open the project in Cursor IDE:" $Yellow
    Write-ColorOutput "   cursor ." $Yellow
    Write-ColorOutput "   OR" $Yellow
    Write-ColorOutput "   Double-click Cursor and open the project folder" $Yellow
    Write-ColorOutput "`n2. Install recommended C++ extensions in Cursor:" $Yellow
    Write-ColorOutput "   - C/C++ (ms-vscode.cpptools)" $Yellow
    Write-ColorOutput "   - CMake Tools (ms-vscode.cmake-tools)" $Yellow
    Write-ColorOutput "   - CMake (twxs.cmake)" $Yellow
    Write-ColorOutput "   - Clang-Format (xaver.clang-format)" $Yellow
    Write-ColorOutput "`n3. Build the project:" $Yellow
    Write-ColorOutput "   .\build.bat" $Yellow
    Write-ColorOutput "   OR use Cursor's built-in CMake integration" $Yellow
    Write-ColorOutput "   OR" $Yellow
    Write-ColorOutput "   cmake -B build -S ." $Yellow
    Write-ColorOutput "   cmake --build build --config Debug" $Yellow
    Write-ColorOutput "`n4. Run the project:" $Yellow
    Write-ColorOutput "   .\run.bat" $Yellow
    Write-ColorOutput "   OR use Cursor's debugger (F5)" $Yellow
    Write-ColorOutput "   OR" $Yellow
    Write-ColorOutput "   .\build\bin\Debug\Tempest.exe" $Yellow
    Write-ColorOutput "`n5. Read the documentation:" $Yellow
    Write-ColorOutput "   docs\README.md" $Yellow
    Write-ColorOutput "   docs\architecture.md" $Yellow
    Write-ColorOutput "   docs\implementation-plan.md" $Yellow
    Write-ColorOutput "`n6. Start development following the implementation plan!" $Yellow
    Write-ColorOutput "`nHappy coding! ðŸŽ®" $Green
}

# Main execution
function Main {
    Write-ColorOutput "=== Tempest Rebuild Project - Windows 11 Development Setup ===" $Blue
    Write-ColorOutput "This script will set up a complete development environment for the Tempest rebuild project." $Yellow
    Write-ColorOutput "`nPrerequisites:" $Blue
    Write-ColorOutput "- Windows 11 (or Windows 10)" $Yellow
    Write-ColorOutput "- Internet connection" $Yellow
    Write-ColorOutput "- Administrator privileges (recommended)" $Yellow
    Write-ColorOutput "- At least 10GB free disk space" $Yellow
    
    if (-not $Force) {
        $response = Read-Host "`nContinue with installation? (y/N)"
        if ($response -ne "y" -and $response -ne "Y") {
            Write-ColorOutput "Installation cancelled." $Red
            exit 1
        }
    }
    
    # Check if running as administrator
    if (-not (Test-Admin)) {
        Write-ColorOutput "Warning: Not running as administrator. Some installations may fail." $Yellow
        $response = Read-Host "Continue anyway? (y/N)"
        if ($response -ne "y" -and $response -ne "Y") {
            exit 1
        }
    }
    
    try {
        # Install components based on parameters
        if (-not $SkipChocolatey) { Install-Chocolatey }
        if (-not $SkipGit) { Install-Git }
        if (-not $SkipVisualStudio) { Install-VisualStudio }
        if (-not $SkipCMake) { Install-CMake }
        if (-not $SkipCursor) { Install-Cursor }
        if (-not $SkipLibraries) { 
            Install-Vcpkg
            Install-Libraries
        }
        if (-not $SkipConfig) {
            Setup-ProjectStructure
            Create-CMakeLists
            Create-CursorConfig
            Create-GitConfig
            Create-BuildScript
        }
        
        # Test the environment
        Test-Environment
        
        # Show next steps
        Show-NextSteps
        
    }
    catch {
        Write-ColorOutput "Error during installation: $($_.Exception.Message)" $Red
        Write-ColorOutput "Please check the error and try again." $Red
        exit 1
    }
}

# Run the main function
Main } 
 
