#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <ctime>
#include <cstdint>
#include <SFML/Graphics.hpp>

#include "core/Window.hpp"
#include "core/Input.hpp"
#include "render/Shader.hpp"
#include "render/Camera.hpp"
#include "render/Renderer.hpp"
#include "render/ChunkRenderer.hpp"
#include "render/ChunkMesher.hpp"
#include "render/Mesh.hpp"
#include "render/Primitives.hpp"
#include "world/Chunk.hpp"
#include "world/BlockRegistry.hpp"
#include "world/WorldGenerator.hpp"
#include "world/FeatureDecorator.hpp"
#include "world/World.hpp"

int main() {

    // === 1. ENGINE INFRASTRUCTURE INITIALIZATION ===

    Window window;
    if (!window.Create(1920, 1080, "Production Architecture Voxel Workspace")) {
        std::cerr << "CRITICAL: Failed to construct primary window layer!" << std::endl;
        return -1;
    }

    // Logical low-res dimensions for chunky retro pixel-art scaling metrics
    const unsigned int logicalWidth  = 960;
    const unsigned int logicalHeight = 540;

    // Explicitly request a 24-bit hardware depth buffer for 3D sorting tests
    sf::ContextSettings textureSettings;
    textureSettings.depthBits = 24;

    // SFML 3.0: Target Framebuffer initialization using uniform brace notation
    sf::RenderTexture gameRenderBuffer({logicalWidth, logicalHeight}, textureSettings);
    gameRenderBuffer.setSmooth(false); // Sharp, crisp nearest-neighbor block edges

    // Establish upscale blit sprite card mapped to the low-res texture attachment
    sf::Sprite upscaleSprite(gameRenderBuffer.getTexture());
    
    // Pin scaling attributes to cleanly stretch 480x270 up to a native 1080p display
    upscaleSprite.setScale({
        1920.0f / static_cast<float>(logicalWidth),
        1080.0f / static_cast<float>(logicalHeight)
    });
    upscaleSprite.setOrigin({0.0f, 0.0f});
    upscaleSprite.setPosition({0.0f, 0.0f});

    // -------------------------------------------------------------------------
    // CRITICAL CONTEXT ALIGNMENT PASS:
    // Lock the RenderTexture's OpenGL context active BEFORE compiling shaders,
    // data registries, or chunk VBO storage models.
    // -------------------------------------------------------------------------
    (void)gameRenderBuffer.setActive(true);

    // Populate flying-weight structural elements in the active buffer context
    World::BlockRegistry::Initialize();

    Renderer renderer;
    Render::ChunkRenderer chunkRenderer;
    Shader isoShader;

    if (!isoShader.LoadFromFile("assets/shaders/isometric.vert", "assets/shaders/isometric.frag")) {
        std::cerr << "CRITICAL: Shader compilation runtime fault detected!" << std::endl;
        return -1;
    }

    // Camera initializes matching the tight logical rendering bounds
    Camera camera(static_cast<float>(logicalWidth), static_cast<float>(logicalHeight));
    
    uint32_t runtimeSeed = static_cast<uint32_t>(std::time(nullptr));
    std::cout << "WORLD PIPELINE INITIATED: Running Seed -> " << runtimeSeed << std::endl;

    World::World gameWorld;
    gameWorld.GenerateDiorama(12, 6, 12, runtimeSeed);

    // Dynamically shift camera destination vector to look at the map midpoint coordinates
    float islandCenter = (12.0f * static_cast<float>(World::CHUNK_SIZE) * 2.0f) / 2.0f;
    camera.SetTarget(glm::vec3(islandCenter, 16.0f, islandCenter));

    bool renderWireframe = false;

    // === 2. CORE RUNTIME APPLICATION LOOP ===

    while (window.IsOpen()) {
        // Gather operating system messaging loops and fill input registries
        Input::UpdateStates();
        window.ProcessFrame();

        // Infrastructure global key triggers
        if (Input::IsKeyHeld(sf::Keyboard::Key::Escape)) {
            window.Close();
        }
        if (Input::IsKeyDown(sf::Keyboard::Key::Space)) {
            renderWireframe = !renderWireframe;
        }

        // Alter camera spatial values on the CPU
        camera.ProcessInput();
        
        // ---------------------------------------------------------------------
        // PASS A: RENDER THE 3D VOXEL WORLD TO LOW-RES HARDWARE FRAMEBUFFER
        // ---------------------------------------------------------------------
        (void)gameRenderBuffer.setActive(true);
        glViewport(0, 0, static_cast<GLsizei>(logicalWidth), static_cast<GLsizei>(logicalHeight));

        // SFML 3.0: Explicitly clear target FBO surface color to diagnostic Neon Crimson
        gameRenderBuffer.clear(sf::Color(30, 33, 45)); 

        // Secure state attributes (forces drivers to respect 3D layouts over 2D leaks)
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); // Allows glClear to completely sweep out the depth field
        glDisable(GL_CULL_FACE); // Temporarily disabled to rule out vertex winding inversion bugs

        // Safely generate vertex stream buffers within the current active target context
        gameWorld.RebuildDirtyMeshes();

        renderer.BeginFrame(); // Clears raw OpenGL depth tracks completely

        if (renderWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Dispatch mesh drawing pipelines
        chunkRenderer.Render(gameWorld.GetChunks(), gameWorld.GetRenderStates(), isoShader, camera);        
        
        // Rasterize graphics operations down onto the internal pixel sheets
        gameRenderBuffer.display();

        // ---------------------------------------------------------------------
        // PASS B: BLIT AND UPSCALE THE TEXTURE MATRIX TO NATIVE 1080P WINDOW
        // ---------------------------------------------------------------------
       (void)window.GetSFMLWindow().setActive(true);
        glViewport(0, 0, 1920, 1080);
        
        glClearColor(0.0f, 0.4f, 1.0f, 1.0f); // Electric Blue clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Purge raw modern 3D configurations
        glUseProgram(0);                  
        glBindVertexArray(0);             
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        
        // FIX: Invalidate SFML's state cache and force it to re-prime its 2D pipeline
        window.GetSFMLWindow().resetGLStates();

        window.GetSFMLWindow().pushGLStates();
        window.GetSFMLWindow().draw(upscaleSprite); 
        window.GetSFMLWindow().popGLStates();
        
        window.Display();
    }

    return 0;
}