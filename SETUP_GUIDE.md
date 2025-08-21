# Quick Setup Guide - SFML Manual Installation

## 📋 **What You Need**

Since you've downloaded SFML, you now need:

1. ✅ **SFML** (you have this)
2. ❓ **nlohmann/json library**
3. ❓ **MinGW compiler**

## 🛠️ **Step 1: Complete Your SFML Setup**

### Find Your SFML Location
Check where you extracted SFML. It should be something like:
- `C:\SFML-2.6.1`
- `C:\SFML-2.6.1-mingw-64`
- `C:\SFML-2.6.1-mingw-32`

The folder should contain:
```
SFML/
├── bin/          (DLL files)
├── include/      (header files)
├── lib/          (library files)
└── ...
```

## 🛠️ **Step 2: Download nlohmann/json**

### Option A: Single Header (Easiest)
1. Go to: https://github.com/nlohmann/json/releases
2. Download `json.hpp` from the latest release
3. Create folder: `C:\nlohmann-json\include\nlohmann\`
4. Put `json.hpp` inside that folder

### Option B: Full Package
1. Download the full source from GitHub
2. Extract to `C:\nlohmann-json\`
3. The include files should be in `C:\nlohmann-json\include\nlohmann\`

## 🛠️ **Step 3: Install MinGW (if not installed)**

### Check if you have MinGW:
```cmd
mingw32-make --version
g++ --version
```

### If not installed:
1. Download from: https://www.mingw-w64.org/downloads/
2. Or use MSYS2: https://www.msys2.org/
3. Make sure `mingw32-make` and `g++` are in your PATH

## 🛠️ **Step 4: Update Paths in Project**

Edit `build.bat` and update these paths to match your setup:

```batch
REM Update this line with your SFML path:
-DSFML_ROOT="C:/SFML-2.6.1"

REM Update this line with your nlohmann/json path:
-DNLOHMANN_JSON_INCLUDE_DIR="C:/nlohmann-json/include"
```

## 🚀 **Step 5: Build the Game**

### Method 1: Use the Build Script
```cmd
# Double-click build.bat
# OR run from command prompt:
build.bat
```

### Method 2: Manual Build
```cmd
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DSFML_ROOT="C:/YOUR_SFML_PATH" -DNLOHMANN_JSON_INCLUDE_DIR="C:/nlohmann-json/include"
mingw32-make
```

## 🎮 **Step 6: Run the Game**

```cmd
cd build
MiniGameSFML.exe
```

## 🔧 **Troubleshooting**

### **Error: "SFML not found"**
- Check your SFML path in `build.bat`
- Make sure the path contains `include/SFML/` and `lib/` folders

### **Error: "nlohmann/json.hpp not found"**
- Download `json.hpp` from GitHub releases
- Put it in `C:\nlohmann-json\include\nlohmann\json.hpp`
- Update the path in `build.bat`

### **Error: "mingw32-make not found"**
- Install MinGW-w64 or MSYS2
- Add MinGW bin directory to your PATH environment variable

### **Error: "DLL not found" when running**
- Copy SFML DLL files from `SFML/bin/` to your `build/` directory
- Or add SFML bin directory to your PATH

### **Missing DLLs (common ones):**
```
sfml-graphics-2.dll
sfml-window-2.dll
sfml-audio-2.dll
sfml-system-2.dll
```

## 📁 **Expected File Structure After Setup**

```
PROJECT/
├── build.bat                 (build script)
├── CMakeLists.txt            (updated with paths)
├── build/                    (created after build)
│   ├── MiniGameSFML.exe     (your game!)
│   └── *.dll                (SFML DLLs if needed)
├── src/                      (source code)
├── include/                  (headers)
└── assets/                   (game assets)

External Dependencies:
C:/SFML-2.6.1/               (your SFML installation)
C:/nlohmann-json/include/    (JSON library)
```

## 🎯 **Quick Test**

After building, you should see:
1. `build/MiniGameSFML.exe` created
2. Game window opens when you run it
3. You can navigate the menu with arrow keys and Enter

## 💡 **Tips**

1. **Use absolute paths** in the build script to avoid confusion
2. **Copy SFML DLLs** to build directory if you get runtime errors
3. **Check MinGW version** - use the same architecture (32/64-bit) for SFML and MinGW
4. **Test with a simple SFML program** first if you have issues

## 🆘 **Still Having Issues?**

Tell me:
1. Where you extracted SFML (exact path)
2. Any error messages you're getting
3. Your MinGW version (`g++ --version`)

I'll help you fix the specific issues!
