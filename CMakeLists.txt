cmake_minimum_required(VERSION 3.20)
project(IsometricVoxelEngine VERSION 1.0 LANGUAGES CXX C)

include(FetchContent)

# ==============================================================================
# 1. DEPENDENCY MANAGEMENT (Find Packages)
# ==============================================================================

find_package(SFML 3.0 REQUIRED COMPONENTS System Window Graphics REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

# ==============================================================================
# 2. THIRD-PARTY TARGET ISOLATION (GLAD Static Library)
# ==============================================================================

add_library(glad STATIC src/glad.c)
target_include_directories(glad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(glad PRIVATE ${OPENGL_LIBRARIES})

# ==============================================================================
# 3. GAME ENGINE EXECUTABLE TARGET & SOURCES
# ==============================================================================

add_executable(${PROJECT_NAME})

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
    src/world/BlockRegistry.cpp
    src/math/NoiseMath.cpp
    src/world/WorldGenerator.cpp
    src/world/World.cpp
    src/render/TextureManager.cpp
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

target_link_libraries(${PROJECT_NAME} PRIVATE
    glad
    SFML::Window
    SFML::System
    SFML::Graphics
    OpenGL::GL
    glm::glm
)