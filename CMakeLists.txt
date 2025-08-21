cmake_minimum_required(VERSION 3.16)
project(MiniGameSFML VERSION 1.0.0 LANGUAGES CXX)

# ---- C++ standard -------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ưu tiên tìm package trong MSYS2 UCRT64 khi dùng MinGW
if (MINGW)
  list(PREPEND CMAKE_PREFIX_PATH "C:/msys64/ucrt64")
endif()

# ---- Sources / Headers ---------------------------------------------------
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
  src/ui/Button.cpp
  src/ui/Panel.cpp
  src/ui/TextLabel.cpp
  src/ui/Bar.cpp
)

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

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ---- SFML (CONFIG) ------------------------------------------------------
# MSYS2/UCRT64 cung cấp config package cho SFML
find_package(SFML 2.6 CONFIG REQUIRED COMPONENTS system window graphics audio)

# Lưu ý: trên MSYS2 các targets là chữ thường: sfml-system/window/graphics/audio
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
)

# ---- nlohmann_json ------------------------------------------------------
# Thử dùng config trước (MSYS2 có gói CONFIG)
find_package(nlohmann_json CONFIG QUIET)
if (nlohmann_json_FOUND)
  target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
else()
  # Fallback header-only (chỉnh đường dẫn nếu bạn đặt khác)
  set(NLOHMANN_JSON_INCLUDE_DIR "C:/msys64/ucrt64/include" CACHE PATH "nlohmann/json include directory")
  target_include_directories(${PROJECT_NAME} PRIVATE ${NLOHMANN_JSON_INCLUDE_DIR})
endif()

# ---- Assets --------------------------------------------------------------
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# ---- Copy runtime DLLs (Windows) ----------------------------------------
if (WIN32 AND MINGW)
  set(SFML_BIN_DIR "C:/msys64/ucrt64/bin")

  # SFML + GCC runtime (tên cố định trong MSYS2)
  foreach(name
      libsfml-system-2.dll libsfml-window-2.dll libsfml-graphics-2.dll libsfml-audio-2.dll
      libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
    if (EXISTS "${SFML_BIN_DIR}/${name}")
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${SFML_BIN_DIR}/${name}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${name}")
    else()
      message(WARNING "Missing runtime: ${name} (in ${SFML_BIN_DIR})")
    endif()
  endforeach()

  # OpenAL: tên khác nhau tùy gói
  set(_OPENAL_CANDIDATES openal32.dll libopenal-1.dll libOpenAL32.dll)
  set(_OPENAL_FOUND 0)
  foreach(name ${_OPENAL_CANDIDATES})
    if (EXISTS "${SFML_BIN_DIR}/${name}")
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${SFML_BIN_DIR}/${name}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${name}")
      set(_OPENAL_FOUND 1)
      break()
    endif()
  endforeach()
  if (NOT _OPENAL_FOUND)
    message(WARNING "OpenAL runtime not found in ${SFML_BIN_DIR}. Install: mingw-w64-ucrt-x86_64-openal")
  endif()

  # libsndfile: đôi khi đổi version suffix
  set(_SNDFILE_CANDIDATES libsndfile-1.dll libsndfile-2.dll)
  set(_SNDFILE_FOUND 0)
  foreach(name ${_SNDFILE_CANDIDATES})
    if (EXISTS "${SFML_BIN_DIR}/${name}")
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${SFML_BIN_DIR}/${name}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${name}")
      set(_SNDFILE_FOUND 1)
      break()
    endif()
  endforeach()
  if (NOT _SNDFILE_FOUND)
    message(WARNING "libsndfile runtime not found. Install: mingw-w64-ucrt-x86_64-libsndfile")
  endif()

  # Các thư viện âm thanh phụ (nếu có thì copy, thiếu cũng không fail)
  foreach(name libFLAC-12.dll libogg-0.dll libvorbis-0.dll libvorbisenc-2.dll libvorbisfile-3.dll libmpg123-0.dll)
    if (EXISTS "${SFML_BIN_DIR}/${name}")
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${SFML_BIN_DIR}/${name}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${name}")
    endif()
  endforeach()
endif()
