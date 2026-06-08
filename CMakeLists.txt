cmake_minimum_required(VERSION 3.20)
project(IsometricVoxelEngine VERSION 1.0 LANGUAGES CXX C)

# ==============================================================================
# 1. DEPENDENCY MANAGEMENT (Find Packages)
# ==============================================================================

find_package(SFML 3.0 REQUIRED COMPONENTS System Window)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

# ==============================================================================
# 2. THIRD-PARTY TARGET ISOLATION (GLAD Static Library)
# ==============================================================================

# Isolate GLAD into its own library so it doesn't inherit your strict compiler warnings
add_library(glad STATIC src/glad.c)
target_include_directories(glad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(glad PRIVATE ${OPENGL_LIBRARIES})

# ==============================================================================
# 3. GAME ENGINE EXECUTABLE TARGET & SOURCES
# ==============================================================================

add_executable(${PROJECT_NAME})

# Explicitly list only YOUR engine source structures here
target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
    src/core/Window.cpp
    src/core/Input.cpp
    src/render/Shader.cpp
    src/render/Mesh.cpp
    src/render/Camera.cpp
    src/render/Renderer.cpp
    src/render/ChunkRenderer.cpp
    src/render/ChunkMesher.cpp
)

# ==============================================================================
# 4. COMPILATION REQUIREMENTS & WARNING DEFENSES
# ==============================================================================

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    # These strict defenses now apply EXCLUSIVELY to your engine code files
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
        -Wshadow         
        -Wconversion     
    )
endif()

# ==============================================================================
# 5. LINK DEPENDENCIES
# ==============================================================================

# Link your custom engine directly to your isolated glad target library
target_link_libraries(${PROJECT_NAME} PRIVATE
    glad
    SFML::Window
    SFML::System
    OpenGL::GL
    glm::glm
)