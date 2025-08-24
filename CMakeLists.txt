cmake_minimum_required(VERSION 3.16)
project(MiniGameSFML VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prefer SFML from MSYS2 (ucrt64) via CMake config; fallback to manual if needed
find_package(SFML 2.6 QUIET COMPONENTS system window graphics audio)
if(SFML_FOUND)
    message(STATUS "Found SFML via CMake: ${SFML_DIR}")
else()
    # Manual fallback (user-provided path)
    set(SFML_ROOT "C:/SFML/SFML-2.6.2" CACHE PATH "SFML root directory")
    set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
    set(SFML_LIBRARY_DIR "${SFML_ROOT}/lib")
    if(NOT EXISTS "${SFML_INCLUDE_DIR}")
        message(FATAL_ERROR "SFML include directory not found: ${SFML_INCLUDE_DIR}")
    endif()
    if(NOT EXISTS "${SFML_LIBRARY_DIR}")
        message(FATAL_ERROR "SFML library directory not found: ${SFML_LIBRARY_DIR}")
    endif()
    include_directories(${SFML_INCLUDE_DIR})
    link_directories(${SFML_LIBRARY_DIR})
    message(STATUS "Using SFML from: ${SFML_ROOT}")
    message(STATUS "SFML include dir: ${SFML_INCLUDE_DIR}")
    message(STATUS "SFML library dir: ${SFML_LIBRARY_DIR}")
endif()

# Find nlohmann_json (or use manual include)
find_package(nlohmann_json 3.2.0 QUIET)
if(NOT nlohmann_json_FOUND)
    # Manual include path for nlohmann/json
    set(NLOHMANN_JSON_INCLUDE_DIR "C:/nlohmann-json/include" CACHE PATH "nlohmann/json include directory")
    if(EXISTS "${NLOHMANN_JSON_INCLUDE_DIR}/nlohmann/json.hpp")
        message(STATUS "Found nlohmann/json at ${NLOHMANN_JSON_INCLUDE_DIR}")
        include_directories(${NLOHMANN_JSON_INCLUDE_DIR})
    else()
        message(WARNING "nlohmann/json not found. Please download json.hpp and place it in ${NLOHMANN_JSON_INCLUDE_DIR}/nlohmann/")
    endif()
endif()

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/StateStack.cpp
    src/core/RNG.cpp
    src/core/AudioManager.cpp
    src/core/AssetManager.cpp
    src/core/SaveSystem.cpp
    src/states/State.cpp
    src/states/MenuState.cpp
    src/states/MapState.cpp
    src/states/DiceState.cpp
    src/states/CoinState.cpp
    src/states/CombatState.cpp
    # src/states/ReadyState.cpp
    src/states/GameOverState.cpp
    src/states/VictoryState.cpp
    src/states/PauseState.cpp
    src/world/Map.cpp
    src/world/Tile.cpp
    src/entities/Entity.cpp
    src/entities/Player.cpp
    src/entities/Enemy.cpp
    src/entities/Boss.cpp
    src/entities/Pokemon.cpp
    src/entities/Skill.cpp
    # src/entities/Pikachu.cpp
    # src/entities/Chalamander.cpp
    # src/entities/Bisasam.cpp
    src/ui/Button.cpp
    src/ui/Panel.cpp
    src/ui/TextLabel.cpp
    src/ui/Bar.cpp
    src/ui/CombatUI.cpp
)

# Header files
set(HEADERS
    include/Game.h
    include/StateStack.h
    include/core/RNG.h
    include/core/AudioManager.h
    include/core/AssetManager.h
    include/core/SaveSystem.h
    include/states/State.h
    include/states/MenuState.h
    include/states/MapState.h
    include/states/DiceState.h
    include/states/CoinState.h
    include/states/CombatState.h
    include/states/GameOverState.h
    include/states/VictoryState.h
    include/states/PauseState.h
    include/world/Map.h
    include/world/Tile.h
    include/entities/Entity.h
    include/entities/Player.h
    include/entities/Enemy.h
    include/entities/Boss.h
    include/entities/Pokemon.h
    include/entities/Skill.h
    include/ui/Button.h
    include/ui/Panel.h
    include/ui/TextLabel.h
    include/ui/Bar.h
    include/Constants.h
    include/Types.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link SFML (prefer standard target names that work across distros)
if(SFML_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-audio sfml-system)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-audio sfml-system)
endif()

# Link nlohmann_json if found via package manager
if(nlohmann_json_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
endif()

# Copy SFML DLLs to build directory (Windows)
if(WIN32)
    if(SFML_FOUND)
        # Try to use MSYS2 default bin path for ucrt64
        set(SFML_DLL_DIR "C:/msys64/ucrt64/bin")
    else()
        set(SFML_DLL_DIR "${SFML_ROOT}/bin")
    endif()
    if(EXISTS "${SFML_DLL_DIR}")
        file(GLOB SFML_DLLS "${SFML_DLL_DIR}/*.dll")
        foreach(dll ${SFML_DLLS})
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${dll} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
        endforeach()
        message(STATUS "SFML DLLs will be copied from: ${SFML_DLL_DIR}")
    else()
        message(WARNING "SFML DLL directory not found: ${SFML_DLL_DIR}")
    endif()
endif()

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Compiler-specific options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Debug/Release configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()
