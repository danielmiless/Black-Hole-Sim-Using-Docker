cmake_minimum_required(VERSION 3.16)
project(BlackHole3D VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${OPENGL_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)

# Collect source files
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.h"
)

# Remove main.cpp from sources for potential library creation
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)

# Create executable
add_executable(black_hole_3d
    src/main.cpp
    ${SOURCES}
)

# Link libraries
target_link_libraries(black_hole_3d
    ${OPENGL_LIBRARIES}
    glfw
    GLEW::GLEW
    GL
    pthread
    dl
    nlohmann_json::nlohmann_json
)

# Copy shader files to build directory
file(COPY shaders/ DESTINATION ${CMAKE_BINARY_DIR}/shaders/)
file(COPY config/ DESTINATION ${CMAKE_BINARY_DIR}/config/)

# Installation rules
install(TARGETS black_hole_3d
    RUNTIME DESTINATION bin
)

install(DIRECTORY shaders/
    DESTINATION share/black_hole_3d/shaders/
)

install(DIRECTORY config/
    DESTINATION share/black_hole_3d/config/
)
