cmake_minimum_required(VERSION 3.16)
project(SDL2_TD VERSION 1.0 LANGUAGES CXX)

# ===============================
# Standard C++
# ===============================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===============================
# Trouver Raylib
# ===============================
find_package(raylib REQUIRED)

# ===============================
# Sources du projet
# ===============================
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# ===============================
# Executable principal
# ===============================
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# ===============================
# Lien avec Raylib
# ===============================
target_link_libraries(${PROJECT_NAME}
    raylib
)

# ===============================
# Options de compilation
# ===============================
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

# ===============================
# Debug / Sanitizers (optionnel)
# ===============================
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -g -fsanitize=address)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
endif()