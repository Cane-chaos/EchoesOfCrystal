@echo off
echo Mini Game SFML - Build Script
echo ==============================

REM Check if build directory exists
if not exist build mkdir build
cd build

echo.
echo Configuring CMake...
echo.

REM Configure with CMake (update SFML_ROOT path as needed)
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DSFML_ROOT="C:/SFML/SFML-2.6.2" -DNLOHMANN_JSON_INCLUDE_DIR="C:/nlohmann-json/include"

if %ERRORLEVEL% neq 0 (
    echo.
    echo ERROR: CMake configuration failed!
    echo.
    echo Please check:
    echo 1. SFML path is correct (update SFML_ROOT in this script)
    echo 2. nlohmann/json is downloaded and path is correct
    echo 3. MinGW is installed and in PATH
    echo.
    pause
    exit /b 1
)

echo.
echo Building project...
echo.

REM Build the project
mingw32-make

if %ERRORLEVEL% neq 0 (
    echo.
    echo ERROR: Build failed!
    echo Check the error messages above.
    echo.
    pause
    exit /b 1
)

echo.
echo ==============================
echo Build completed successfully!
echo ==============================
echo.
echo Executable location: build\MiniGameSFML.exe
echo.
echo To run the game:
echo   cd build
echo   MiniGameSFML.exe
echo.
pause
