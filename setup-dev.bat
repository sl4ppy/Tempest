@echo off
echo Tempest Rebuild Project - Development Environment Setup
echo ======================================================
echo.
echo This script will set up a complete development environment for the Tempest rebuild project.
echo.
echo Prerequisites:
echo - Windows 11 (or Windows 10)
echo - Internet connection
echo - Administrator privileges (recommended)
echo - At least 10GB free disk space
echo.

REM Check if running as administrator
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Warning: Not running as administrator. Some installations may fail.
    echo.
    set /p continue="Continue anyway? (y/N): "
    if /i not "%continue%"=="y" (
        echo Setup cancelled.
        pause
        exit /b 1
    )
)

echo.
echo Starting development environment setup...
echo This may take 30-60 minutes depending on your internet connection.
echo.

REM Set execution policy and run the PowerShell script
powershell -ExecutionPolicy Bypass -File "%~dp0scripts\setup-simple.ps1"

if %errorLevel% neq 0 (
    echo.
    echo Setup failed with error code %errorLevel%
    echo Please check the error messages above and try again.
    pause
    exit /b 1
)

echo.
echo Setup completed successfully!
echo.
echo Next steps:
echo 1. Open the project in Cursor IDE: cursor .
echo 2. Install C++ extensions in Cursor's marketplace
echo 3. Build the project: .\build.bat
echo 4. Run the project: .\run.bat
echo 5. Read the documentation in the docs\ folder
echo.
echo Happy coding! 🎮
pause 