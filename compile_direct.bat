@echo off
echo Mini Game SFML - Direct Compilation (No CMake)
echo ===============================================

REM Check if MinGW is available
g++ --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: g++ not found! Please install MinGW.
    pause
    exit /b 1
)

echo.
echo Compiling all source files...
echo.

REM Create build directory
if not exist build mkdir build

REM Compile all source files directly
g++ -std=c++17 ^
    -I"include" ^
    -I"C:/SFML/include" ^
    -I"C:/nlohmann-json/include" ^
    -L"C:/SFML/lib" ^
    -O2 ^
    src/main.cpp ^
    src/Game.cpp ^
    src/StateStack.cpp ^
    src/core/RNG.cpp ^
    src/core/AudioManager.cpp ^
    src/core/AssetManager.cpp ^
    src/core/SaveSystem.cpp ^
    src/states/State.cpp ^
    src/states/MenuState.cpp ^
    src/states/MapState.cpp ^
    src/states/DiceState.cpp ^
    src/states/CoinState.cpp ^
    src/states/CombatState.cpp ^
    src/states/GameOverState.cpp ^
    src/states/VictoryState.cpp ^
    src/states/PauseState.cpp ^
    src/world/Map.cpp ^
    src/world/Tile.cpp ^
    src/entities/Entity.cpp ^
    src/entities/Player.cpp ^
    src/entities/Enemy.cpp ^
    src/entities/Boss.cpp ^
    src/entities/Pokemon.cpp ^
    src/entities/Skill.cpp ^
    src/ui/Button.cpp ^
    src/ui/Panel.cpp ^
    src/ui/TextLabel.cpp ^
    src/ui/Bar.cpp ^
    -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system ^
    -o build/MiniGameSFML.exe

if %ERRORLEVEL% neq 0 (
    echo.
    echo ERROR: Compilation failed!
    echo.
    pause
    exit /b 1
)

echo.
echo Copying SFML DLLs...
copy "C:\SFML\bin\sfml-graphics-2.dll" "build\" >nul 2>&1
copy "C:\SFML\bin\sfml-window-2.dll" "build\" >nul 2>&1
copy "C:\SFML\bin\sfml-audio-2.dll" "build\" >nul 2>&1
copy "C:\SFML\bin\sfml-system-2.dll" "build\" >nul 2>&1

echo.
echo ===============================================
echo Compilation completed successfully!
echo ===============================================
echo.
echo Executable: build\MiniGameSFML.exe
echo.
echo To run the game:
echo   cd build
echo   MiniGameSFML.exe
echo.
pause
