cmake_minimum_required(VERSION 3.20)
project(IsometricVoxelEngine VERSION 1.0 LANGUAGES CXX C) # Added C for glad.c

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Discover System Packages
find_package(SFML 3.0 REQUIRED COMPONENTS System Window)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

# Point directly to your local project include structures
include_directories(include)

# Gather your custom code structures AND the generated glad.c
file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.c")

if(NOT SRC_FILES)
    message(FATAL_ERROR "No source files found in src/ folder! Make sure src/main.cpp and src/glad.c are present.")
endif()

# Build the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    SFML::Window
    SFML::System
    glm::glm
    ${OPENGL_LIBRARIES}
)