@echo off
echo Building Tempest Rebuild Project...

if not exist build mkdir build
cd build

echo Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/Users/chris/Documents/GitHub/Tempest/tools/vcpkg/scripts/buildsystems/vcpkg.cmake

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
