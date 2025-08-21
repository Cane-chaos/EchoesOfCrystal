# Build Instructions - Mini Game SFML

## Prerequisites

### Required Dependencies
- **CMake 3.16+**
- **C++17 compatible compiler** (GCC 7+, Clang 5+, MSVC 2017+)
- **SFML 2.6.x** (graphics, window, audio)
- **nlohmann/json 3.2.0+**

## Platform-Specific Setup

### Windows (MSVC/MinGW)

#### Option 1: vcpkg (Recommended)
```bash
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install dependencies
.\vcpkg install sfml:x64-windows
.\vcpkg install nlohmann-json:x64-windows

# Build project
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

#### Option 2: Manual Installation
1. Download SFML 2.6.x from https://www.sfml-dev.org/download.php
2. Extract to `C:\SFML` (or preferred location)
3. Download nlohmann/json from https://github.com/nlohmann/json
4. Build:
```bash
mkdir build
cd build
cmake .. -DSFML_ROOT=C:\SFML
cmake --build . --config Release
```

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt update
sudo apt install build-essential cmake
sudo apt install libsfml-dev
sudo apt install nlohmann-json3-dev

# Build project
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Linux (Fedora/CentOS)

```bash
# Install dependencies
sudo dnf install gcc-c++ cmake
sudo dnf install SFML-devel
sudo dnf install json-devel

# Build project
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### macOS

#### Option 1: Homebrew (Recommended)
```bash
# Install dependencies
brew install cmake
brew install sfml
brew install nlohmann-json

# Build project
mkdir build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

#### Option 2: MacPorts
```bash
# Install dependencies
sudo port install cmake
sudo port install sfml
sudo port install nlohmann_json

# Build project
mkdir build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

## Build Configuration

### Debug Build
```bash
mkdir build-debug
cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Release Build
```bash
mkdir build-release
cd build-release
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Custom SFML Path
If SFML is installed in a custom location:
```bash
cmake .. -DSFML_ROOT=/path/to/sfml
```

## Running the Game

### Windows
```bash
cd build
.\MiniGameSFML.exe
```

### Linux/macOS
```bash
cd build
./MiniGameSFML
```

## Troubleshooting

### Common Issues

#### SFML Not Found
- Ensure SFML is properly installed
- Set `SFML_ROOT` environment variable or use `-DSFML_ROOT=` flag
- Check that SFML version is 2.6.x

#### nlohmann/json Not Found
- Install via package manager or vcpkg
- For manual installation, ensure headers are in include path

#### Missing Audio Files
- The game will run without audio files but may show warnings
- Audio files are optional for basic functionality

#### Font Loading Issues
- Default system fonts will be used if custom fonts are not found
- Ensure assets directory is copied to build directory

### Performance Issues
- Use Release build for better performance
- Enable compiler optimizations: `-DCMAKE_BUILD_TYPE=Release`
- Ensure graphics drivers are up to date

## Development Setup

### IDE Configuration

#### Visual Studio Code
1. Install C++ extension
2. Install CMake Tools extension
3. Open project folder
4. Configure CMake kit
5. Build and debug

#### Visual Studio (Windows)
1. Open folder in Visual Studio
2. CMake will be automatically detected
3. Configure CMake settings
4. Build and run

#### CLion
1. Open CMakeLists.txt as project
2. Configure toolchain
3. Build and run

### Adding New Files
1. Add source files to `SOURCES` list in CMakeLists.txt
2. Add header files to `HEADERS` list in CMakeLists.txt
3. Reconfigure CMake: `cmake ..`

## Asset Management

The game expects the following directory structure:
```
build/
├── MiniGameSFML(.exe)
└── assets/
    ├── fonts/
    │   └── arial.ttf (optional)
    └── audio/
        ├── bgm_menu.ogg (optional)
        ├── bgm_map.ogg (optional)
        ├── bgm_combat.ogg (optional)
        ├── sfx_coin.ogg (optional)
        ├── sfx_dice.ogg (optional)
        └── sfx_hit.ogg (optional)
```

Assets are automatically copied during build. All assets are optional - the game will use fallbacks if files are missing.
