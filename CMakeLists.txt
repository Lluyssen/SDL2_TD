cmake_minimum_required(VERSION 3.16)

project(NervousGame VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)


# Raylib

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)


# ECS

FetchContent_Declare(
    ecs_repo
    GIT_REPOSITORY https://github.com/Lluyssen/ECS.git
    GIT_TAG main
)

FetchContent_Populate(ecs_repo)

add_subdirectory(
    ${ecs_repo_SOURCE_DIR}
    ${ecs_repo_BINARY_DIR}
)


# Sources

file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)


# Link

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        raylib
        ECS
)