#!/usr/bin/env python3
"""
Project verification script for Mini Game SFML
Checks that all required files are present and properly structured.
"""

import os
import sys
from pathlib import Path

def check_file_exists(filepath, description=""):
    """Check if a file exists and report status."""
    if os.path.exists(filepath):
        print(f"✓ {filepath} {description}")
        return True
    else:
        print(f"✗ {filepath} {description} - MISSING")
        return False

def check_directory_structure():
    """Verify the project directory structure."""
    print("=== Checking Project Structure ===")
    
    required_files = [
        ("CMakeLists.txt", "- Build configuration"),
        ("README.md", "- Project documentation"),
        ("BUILD.md", "- Build instructions"),
        ("DESIGN.md", "- Technical design document"),
    ]
    
    required_dirs = [
        ("assets", "- Asset directory"),
        ("assets/fonts", "- Font assets"),
        ("assets/audio", "- Audio assets"),
        ("include", "- Header files"),
        ("src", "- Source files"),
    ]
    
    # Check files
    all_good = True
    for filepath, desc in required_files:
        if not check_file_exists(filepath, desc):
            all_good = False
    
    # Check directories
    for dirpath, desc in required_dirs:
        if not check_file_exists(dirpath, desc):
            all_good = False
    
    return all_good

def check_header_files():
    """Check that all required header files exist."""
    print("\n=== Checking Header Files ===")
    
    headers = [
        "include/Game.h",
        "include/StateStack.h",
        "include/Constants.h",
        "include/Types.h",
        "include/core/RNG.h",
        "include/core/AudioManager.h",
        "include/core/AssetManager.h",
        "include/core/SaveSystem.h",
        "include/states/State.h",
        "include/states/MenuState.h",
        "include/states/MapState.h",
        "include/states/DiceState.h",
        "include/states/CoinState.h",
        "include/states/CombatState.h",
        "include/states/GameOverState.h",
        "include/states/VictoryState.h",
        "include/states/PauseState.h",
        "include/world/Map.h",
        "include/world/Tile.h",
        "include/entities/Entity.h",
        "include/entities/Player.h",
        "include/entities/Enemy.h",
        "include/entities/Boss.h",
        "include/entities/Pokemon.h",
        "include/entities/Skill.h",
        "include/ui/Button.h",
        "include/ui/Panel.h",
        "include/ui/TextLabel.h",
        "include/ui/Bar.h",
    ]
    
    all_good = True
    for header in headers:
        if not check_file_exists(header):
            all_good = False
    
    return all_good

def check_source_files():
    """Check that all required source files exist."""
    print("\n=== Checking Source Files ===")
    
    sources = [
        "src/main.cpp",
        "src/Game.cpp",
        "src/StateStack.cpp",
        "src/core/RNG.cpp",
        "src/core/AudioManager.cpp",
        "src/core/AssetManager.cpp",
        "src/core/SaveSystem.cpp",
        "src/states/State.cpp",
        "src/states/MenuState.cpp",
        "src/states/MapState.cpp",
        "src/states/DiceState.cpp",
        "src/states/CoinState.cpp",
        "src/states/CombatState.cpp",
        "src/states/GameOverState.cpp",
        "src/states/VictoryState.cpp",
        "src/states/PauseState.cpp",
        "src/world/Map.cpp",
        "src/world/Tile.cpp",
        "src/entities/Entity.cpp",
        "src/entities/Player.cpp",
        "src/entities/Enemy.cpp",
        "src/entities/Boss.cpp",
        "src/entities/Pokemon.cpp",
        "src/entities/Skill.cpp",
        "src/ui/Button.cpp",
        "src/ui/Panel.cpp",
        "src/ui/TextLabel.cpp",
        "src/ui/Bar.cpp",
    ]
    
    all_good = True
    for source in sources:
        if not check_file_exists(source):
            all_good = False
    
    return all_good

def count_lines_of_code():
    """Count total lines of code in the project."""
    print("\n=== Code Statistics ===")
    
    total_lines = 0
    file_count = 0
    
    # Count header files
    for root, dirs, files in os.walk("include"):
        for file in files:
            if file.endswith(('.h', '.hpp')):
                filepath = os.path.join(root, file)
                with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                    lines = len(f.readlines())
                    total_lines += lines
                    file_count += 1
    
    # Count source files
    for root, dirs, files in os.walk("src"):
        for file in files:
            if file.endswith(('.cpp', '.c')):
                filepath = os.path.join(root, file)
                with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                    lines = len(f.readlines())
                    total_lines += lines
                    file_count += 1
    
    print(f"Total files: {file_count}")
    print(f"Total lines of code: {total_lines}")
    
    return total_lines, file_count

def main():
    """Main verification function."""
    print("Mini Game SFML - Project Verification")
    print("=" * 40)
    
    # Change to project directory if script is run from elsewhere
    script_dir = Path(__file__).parent
    os.chdir(script_dir)
    
    # Run all checks
    structure_ok = check_directory_structure()
    headers_ok = check_header_files()
    sources_ok = check_source_files()
    
    # Count code
    lines, files = count_lines_of_code()
    
    # Final report
    print("\n=== Final Report ===")
    if structure_ok and headers_ok and sources_ok:
        print("✓ All required files are present!")
        print("✓ Project structure is complete!")
        print(f"✓ Ready for compilation with {files} files and {lines} lines of code")
        print("\nNext steps:")
        print("1. Install SFML 2.6.x and nlohmann/json")
        print("2. Run: mkdir build && cd build")
        print("3. Run: cmake ..")
        print("4. Run: make (Linux/Mac) or cmake --build . (Windows)")
        return 0
    else:
        print("✗ Some files are missing!")
        print("✗ Project structure is incomplete!")
        return 1

if __name__ == "__main__":
    sys.exit(main())
