cmake_minimum_required(VERSION 3.16)

project(NervousGame VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# =========================
# Raylib
# =========================
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

# =========================
# ECS (via FetchContent)
# =========================
FetchContent_Declare(
    ecs
    GIT_REPOSITORY https://github.com/Lluyssen/ECS.git
    GIT_TAG main
)
FetchContent_MakeAvailable(ecs)

# =========================
# JSON
# =========================

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(json)

# =========================
# Sources du projet
# =========================
file(GLOB_RECURSE SRC_FILES src/main.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# =========================
# Link
# =========================
target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
    ECS
    nlohmann_json::nlohmann_json
)