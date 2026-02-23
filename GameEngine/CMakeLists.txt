cmake_minimum_required(VERSION 3.20)
project(EduEngine VERSION 1.0.0 LANGUAGES CXX)

# ── C++17 standard — required for std::type_index, fold expressions, etc. ──
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS  OFF)

# ── Build type defaults ──────────────────────────────────────────────────────
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler:   ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# ── Compiler warnings ────────────────────────────────────────────────────────
if(MSVC)
  add_compile_options(/W4 /WX /permissive-)
else()
  add_compile_options(
    -Wall -Wextra -Wpedantic
    -Wno-unused-parameter          # ECS systems have intentional unused params
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
  )
endif()

# ════════════════════════════════════════════════════════════════════════════
# Dependencies
# ════════════════════════════════════════════════════════════════════════════

# ── SDL2 ─────────────────────────────────────────────────────────────────────
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf QUIET)   # For text rendering.

# ── nlohmann/json ────────────────────────────────────────────────────────────
# Option A: system-installed (apt install nlohmann-json3-dev)
find_package(nlohmann_json 3.9 QUIET)

# Option B: vendored (copy single-header json.hpp into vendor/nlohmann/)
if(NOT nlohmann_json_FOUND)
  message(STATUS "nlohmann/json not found via find_package — using vendor/nlohmann/")
  add_library(nlohmann_json INTERFACE)
  target_include_directories(nlohmann_json INTERFACE ${CMAKE_SOURCE_DIR}/vendor)
endif()

# ── Dear ImGui ───────────────────────────────────────────────────────────────
# Compile ImGui as a static library from the vendor directory.
# Clone ImGui into vendor/imgui/ :
#   git clone https://github.com/ocornut/imgui vendor/imgui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui)
file(GLOB IMGUI_SOURCES
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
  ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

if(IMGUI_SOURCES)
  add_library(imgui STATIC ${IMGUI_SOURCES})
  target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
  target_link_libraries(imgui PUBLIC SDL2::SDL2)
  message(STATUS "Dear ImGui: found in vendor/imgui/")
else()
  message(FATAL_ERROR
    "Dear ImGui not found. Run:\n"
    "  git clone https://github.com/ocornut/imgui vendor/imgui")
endif()

# ════════════════════════════════════════════════════════════════════════════
# Source files
# ════════════════════════════════════════════════════════════════════════════
set(ENGINE_SOURCES
  # Core
  src/core/Engine.cpp

  # Scene
  src/scene/SceneLoader.cpp

  # Editor
  src/editor/EditorTools.cpp

  # Educational
  src/educational/ReflectionSystem.cpp
  src/educational/AdaptiveController.cpp
  src/educational/RewardEconomy.cpp
)

# ════════════════════════════════════════════════════════════════════════════
# Executable
# ════════════════════════════════════════════════════════════════════════════
add_executable(EduEngine src/main.cpp ${ENGINE_SOURCES})

# ── Include paths ─────────────────────────────────────────────────────────────
target_include_directories(EduEngine PRIVATE
  ${CMAKE_SOURCE_DIR}/src          # Engine headers
  ${CMAKE_SOURCE_DIR}/vendor        # nlohmann/json (if vendored)
)

# ── Link libraries ────────────────────────────────────────────────────────────
target_link_libraries(EduEngine PRIVATE
  SDL2::SDL2
  imgui
  nlohmann_json::nlohmann_json
)

# SDL2_ttf (optional but recommended).
if(SDL2_ttf_FOUND OR TARGET SDL2_ttf::SDL2_ttf)
  target_link_libraries(EduEngine PRIVATE SDL2_ttf::SDL2_ttf)
  target_compile_definitions(EduEngine PRIVATE EDUENGINE_TTF=1)
  message(STATUS "SDL2_ttf: found")
else()
  message(WARNING "SDL2_ttf not found — text rendering will be disabled.")
endif()

# libcurl (optional — for real AI API scoring).
find_package(CURL QUIET)
if(CURL_FOUND)
  target_link_libraries(EduEngine PRIVATE CURL::libcurl)
  target_compile_definitions(EduEngine PRIVATE EDUENGINE_USE_CURL=1)
  message(STATUS "libcurl: found — real AI API scoring enabled.")
else()
  message(STATUS "libcurl: NOT found — mock scoring will be used.")
endif()

# ── Preprocessor definitions ─────────────────────────────────────────────────
target_compile_definitions(EduEngine PRIVATE
  $<$<CONFIG:Debug>:EDUENGINE_DEBUG=1>
  $<$<CONFIG:Release>:EDUENGINE_RELEASE=1>
)

# ── Copy assets and data next to the executable on build ─────────────────────
add_custom_command(TARGET EduEngine POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:EduEngine>/assets
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:EduEngine>/data
  COMMENT "Copying assets and data to build directory..."
)

# ════════════════════════════════════════════════════════════════════════════
# Installation
# ════════════════════════════════════════════════════════════════════════════
install(TARGETS EduEngine DESTINATION bin)
install(DIRECTORY assets/ DESTINATION bin/assets)
install(DIRECTORY data/   DESTINATION bin/data)

# ════════════════════════════════════════════════════════════════════════════
# Summary
# ════════════════════════════════════════════════════════════════════════════
message(STATUS "")
message(STATUS "══════════════════════════════════════")
message(STATUS " EduEngine Configuration Summary")
message(STATUS "══════════════════════════════════════")
message(STATUS " Build type  : ${CMAKE_BUILD_TYPE}")
message(STATUS " Install dir : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "══════════════════════════════════════")
