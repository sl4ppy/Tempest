# Tempest Rebuild Project - Windows 11 Development Environment Setup
# Simplified version to avoid syntax issues

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

# Set execution policy
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser -Force

# Configuration
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$ToolsDir = Join-Path $ProjectRoot "tools"
$VcpkgDir = Join-Path $ToolsDir "vcpkg"

# Colors for output
$Red = "Red"
$Green = "Green"
$Yellow = "Yellow"
$Blue = "Blue"

function Write-ColorOutput {
    param([string]$Message, [string]$Color = "White")
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
    Write-ColorOutput 'Installing Chocolatey package manager...' $Blue
    
    if (Test-Command 'choco') {
        Write-ColorOutput 'Chocolatey is already installed.' $Green
        return
    }
    
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    
    $env:Path = [System.Environment]::GetEnvironmentVariable('Path','Machine') + ';' + [System.Environment]::GetEnvironmentVariable('Path','User')
    
    Write-ColorOutput 'Chocolatey installed successfully.' $Green
}

function Install-Git {
    Write-ColorOutput 'Installing Git...' $Blue
    
    if (Test-Command 'git') {
        Write-ColorOutput 'Git is already installed.' $Green
        return
    }
    
    choco install git -y
    
    git config --global core.autocrlf true
    git config --global core.eol lf
    git config --global init.defaultBranch main
    
    Write-ColorOutput 'Git installed and configured successfully.' $Green
}

function Install-VisualStudio {
    Write-ColorOutput 'Installing Visual Studio 2022 Community...' $Blue
    
    $vsInstallPath = '${env:ProgramFiles}\Microsoft Visual Studio\2022\Community'
    if (Test-Path $vsInstallPath) {
        Write-ColorOutput 'Visual Studio 2022 Community is already installed.' $Green
        return
    }
    
    $vsInstallerUrl = 'https://aka.ms/vs/17/release/vs_community.exe'
    $vsInstallerPath = Join-Path $env:TEMP 'vs_community.exe'
    
    Write-ColorOutput 'Downloading Visual Studio Installer...' $Yellow
    Invoke-WebRequest -Uri $vsInstallerUrl -OutFile $vsInstallerPath
    
    Write-ColorOutput 'Installing Visual Studio (this may take a while)...' $Yellow
    Start-Process -FilePath $vsInstallerPath -ArgumentList @(
        '--quiet',
        '--wait',
        '--norestart',
        '--add Microsoft.VisualStudio.Workload.NativeDesktop',
        '--add Microsoft.VisualStudio.Workload.NativeCrossPlat',
        '--add Microsoft.VisualStudio.Component.CMake.Project',
        '--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
        '--add Microsoft.VisualStudio.Component.Windows10SDK.19041',
        '--add Microsoft.VisualStudio.Component.Git.GitCredentialManager'
    ) -Wait
    
    Remove-Item $vsInstallerPath -Force
    
    Write-ColorOutput 'Visual Studio 2022 Community installed successfully.' $Green
}

function Install-CMake {
    Write-ColorOutput 'Installing CMake...' $Blue
    
    if (Test-Command 'cmake') {
        Write-ColorOutput 'CMake is already installed.' $Green
        return
    }
    
    choco install cmake -y
    
    Write-ColorOutput 'CMake installed successfully.' $Green
}

function Install-Cursor {
    Write-ColorOutput 'Installing Cursor IDE...' $Blue
    
    if (Test-Command 'cursor') {
        Write-ColorOutput 'Cursor is already installed.' $Green
        return
    }
    
    Write-ColorOutput 'Downloading Cursor...' $Yellow
    $cursorUrl = 'https://download.cursor.sh/windows'
    $cursorInstallerPath = Join-Path $env:TEMP 'cursor-setup.exe'
    
    try {
        Invoke-WebRequest -Uri $cursorUrl -OutFile $cursorInstallerPath
        Write-ColorOutput 'Installing Cursor...' $Yellow
        Start-Process -FilePath $cursorInstallerPath -ArgumentList '/S' -Wait
        Remove-Item $cursorInstallerPath -Force
        Write-ColorOutput 'Cursor installed successfully.' $Green
    }
    catch {
        Write-ColorOutput 'Failed to download Cursor automatically. Please install manually from https://cursor.sh' $Yellow
    }
    
    Write-ColorOutput 'Note: Install C++ extensions through Cursor''s extension marketplace after first launch.' $Yellow
}

function Install-Vcpkg {
    Write-ColorOutput 'Installing vcpkg package manager...' $Blue
    
    if (Test-Path $VcpkgDir) {
        Write-ColorOutput 'vcpkg is already installed.' $Green
        return
    }
    
    New-Item -ItemType Directory -Path $ToolsDir -Force | Out-Null
    
    git clone https://github.com/Microsoft/vcpkg.git $VcpkgDir
    
    Push-Location $VcpkgDir
    .\bootstrap-vcpkg.bat
    Pop-Location
    
    $vcpkgPath = Join-Path $VcpkgDir 'installed\x64-windows\tools'
    $env:Path += ';$vcpkgPath'
    [Environment]::SetEnvironmentVariable('Path', $env:Path, 'User')
    
    Write-ColorOutput 'vcpkg installed successfully.' $Green
}

function Install-Libraries {
    Write-ColorOutput 'Installing required libraries...' $Blue
    
    Push-Location $VcpkgDir
    
    Write-ColorOutput 'Installing SDL2...' $Yellow
    .\vcpkg install sdl2:x64-windows
    
    Write-ColorOutput 'Installing OpenAL...' $Yellow
    .\vcpkg install openal-soft:x64-windows
    
    Write-ColorOutput 'Installing GLFW...' $Yellow
    .\vcpkg install glfw3:x64-windows
    
    Write-ColorOutput 'Installing GLM...' $Yellow
    .\vcpkg install glm:x64-windows
    
    Write-ColorOutput 'Installing stb...' $Yellow
    .\vcpkg install stb:x64-windows
    
    Write-ColorOutput 'Installing nlohmann-json...' $Yellow
    .\vcpkg install nlohmann-json:x64-windows
    
    Write-ColorOutput 'Installing spdlog...' $Yellow
    .\vcpkg install spdlog:x64-windows
    
    Write-ColorOutput 'Installing Google Test...' $Yellow
    .\vcpkg install gtest:x64-windows
    
    .\vcpkg integrate install
    
    Pop-Location
    
    Write-ColorOutput 'All libraries installed successfully.' $Green
}

function Setup-ProjectStructure {
    Write-ColorOutput 'Setting up project structure...' $Blue
    
    $directories = @(
        'src', 'src\Core', 'src\Game', 'src\Graphics', 'src\Audio', 'src\Input',
        'src\Platform', 'src\Platform\Windows', 'assets', 'assets\audio',
        'assets\graphics', 'assets\fonts', 'tests', 'tests\unit', 'tests\integration',
        'docs', 'scripts', 'tools', 'build', 'build\Debug', 'build\Release',
        'build\RelWithDebInfo', 'build\MinSizeRel'
    )
    
    foreach ($dir in $directories) {
        $path = Join-Path $ProjectRoot $dir
        New-Item -ItemType Directory -Path $path -Force | Out-Null
    }
    
    Write-ColorOutput 'Project structure created successfully.' $Green
}

function Create-BasicConfig {
    Write-ColorOutput 'Creating basic configuration files...' $Blue
    
    # Create .gitignore
    $gitignoreContent = @'
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
'@
    
    $gitignorePath = Join-Path $ProjectRoot '.gitignore'
    $gitignoreContent | Out-File -FilePath $gitignorePath -Encoding UTF8
    
    # Create simple build script
    $buildBatContent = @'
@echo off
echo Building Tempest Rebuild Project...

if not exist build mkdir build
cd build

echo Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64

if errorlevel 1 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo Building project...
cmake --build . --config Debug

if errorlevel 1 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build completed successfully!
cd ..
pause
'@
    
    $buildBatPath = Join-Path $ProjectRoot 'build.bat'
    $buildBatContent | Out-File -FilePath $buildBatPath -Encoding ASCII
    
    # Initialize Git if not already done
    if (-not (Test-Path (Join-Path $ProjectRoot '.git'))) {
        git init
        git add .
        git commit -m 'Initial commit: Project setup'
    }
    
    Write-ColorOutput 'Basic configuration created successfully.' $Green
}

function Test-Environment {
    Write-ColorOutput 'Testing development environment...' $Blue
    
    $tests = @(
        @{ Name = 'Git'; Command = 'git --version' },
        @{ Name = 'CMake'; Command = 'cmake --version' },
        @{ Name = 'Visual Studio'; Command = '"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\cl.exe"' },
        @{ Name = 'vcpkg'; Command = 'tools\vcpkg\vcpkg version' }
    )
    
    $allPassed = $true
    
    foreach ($test in $tests) {
        try {
            $result = Invoke-Expression $test.Command 2>$null
            if ($LASTEXITCODE -eq 0) {
                Write-ColorOutput "+ $($test.Name): OK" $Green
            } else {
                Write-ColorOutput "- $($test.Name): Failed" $Red
                $allPassed = $false
            }
        }
        catch {
            Write-ColorOutput "- $($test.Name): Failed" $Red
            $allPassed = $false
        }
    }
    
    if ($allPassed) {
        Write-ColorOutput 'All environment tests passed!' $Green
    } else {
        Write-ColorOutput 'Some environment tests failed. Please check the installation.' $Red
    }
    
    return $allPassed
}

function Show-NextSteps {
    Write-ColorOutput '`n=== Development Environment Setup Complete ===' $Green
    Write-ColorOutput '`nNext steps:' $Blue
    Write-ColorOutput '1. Open the project in Cursor IDE: cursor .' $Yellow
    Write-ColorOutput '2. Install C++ extensions in Cursor marketplace' $Yellow
    Write-ColorOutput '3. Build the project: .\build.bat' $Yellow
    Write-ColorOutput '4. Read the documentation in the docs\ folder' $Yellow
    Write-ColorOutput '`nHappy coding! 🎮' $Green
}

# Main execution
function Main {
    Write-ColorOutput '=== Tempest Rebuild Project - Windows 11 Development Setup ===' $Blue
    Write-ColorOutput 'This script will set up a complete development environment for the Tempest rebuild project.' $Yellow
    Write-ColorOutput '`nPrerequisites:' $Blue
    Write-ColorOutput '- Windows 11 (or Windows 10)' $Yellow
    Write-ColorOutput '- Internet connection' $Yellow
    Write-ColorOutput '- Administrator privileges (recommended)' $Yellow
    Write-ColorOutput '- At least 10GB free disk space' $Yellow
    
    if (-not $Force) {
        $response = Read-Host '`nContinue with installation? (y/N)'
        if ($response -ne 'y' -and $response -ne 'Y') {
            Write-ColorOutput 'Installation cancelled.' $Red
            exit 1
        }
    }
    
    if (-not (Test-Admin)) {
        Write-ColorOutput 'Warning: Not running as administrator. Some installations may fail.' $Yellow
        $response = Read-Host 'Continue anyway? (y/N)'
        if ($response -ne 'y' -and $response -ne 'Y') {
            exit 1
        }
    }
    
    try {
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
            Create-BasicConfig
        }
        
        Test-Environment
        Show-NextSteps
        
    }
    catch {
        Write-ColorOutput 'Error during installation: $($_.Exception.Message)' $Red
        Write-ColorOutput 'Please check the error and try again.' $Red
        exit 1
    }
}

# Run the main function
Main 