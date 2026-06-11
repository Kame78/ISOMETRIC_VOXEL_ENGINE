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
#include "world/World.hpp"

int main() {

    // === 1. ENGINE INFRASTRUCTURE INITIALIZATION ===

    Window window;
    if (!window.Create(1920, 1080, "Production Architecture Voxel Workspace")) {
        std::cerr << "CRITICAL: Failed to construct primary window layer!" << std::endl;
        return -1;
    }

    const unsigned int logicalWidth  = 960;
    const unsigned int logicalHeight = 540;

    sf::ContextSettings textureSettings;
    textureSettings.depthBits = 24;

    sf::RenderTexture gameRenderBuffer({logicalWidth, logicalHeight}, textureSettings);
    gameRenderBuffer.setSmooth(false); // Sharp, crisp nearest-neighbor block edges

    sf::Sprite upscaleSprite(gameRenderBuffer.getTexture());
    upscaleSprite.setScale({
        1920.0f / static_cast<float>(logicalWidth),
        1080.0f / static_cast<float>(logicalHeight)
    });
    upscaleSprite.setOrigin({0.0f, 0.0f});
    upscaleSprite.setPosition({0.0f, 0.0f});

    (void)gameRenderBuffer.setActive(true);

    auto blockRegistry = World::BlockOps::LoadRegistryFromFile("assets/block_registry.json");

    Renderer renderer;
    Render::ChunkRenderer chunkRenderer;
    Shader isoShader;

    if (!isoShader.LoadFromFile("assets/shaders/isometric.vert", "assets/shaders/isometric.frag")) {
        std::cerr << "CRITICAL: Shader compilation runtime fault detected!" << std::endl;
        return -1;
    }

    Camera camera(static_cast<float>(logicalWidth), static_cast<float>(logicalHeight));
    
    uint32_t runtimeSeed = static_cast<uint32_t>(std::time(nullptr));
    std::cout << "WORLD PIPELINE INITIATED: Running Seed -> " << runtimeSeed << std::endl;

    World::World gameWorld;
    gameWorld.GenerateDiorama(12, 6, 12, runtimeSeed, blockRegistry);

    float islandCenter = (12.0f * static_cast<float>(World::CHUNK_SIZE) * 2.0f) / 2.0f;
    camera.SetTarget(glm::vec3(islandCenter, 16.0f, islandCenter));

    bool renderWireframe = false;

   while (window.IsOpen()) {
    Input::UpdateStates();
    window.ProcessFrame();

    if (Input::IsKeyHeld(sf::Keyboard::Key::Escape)) {
        window.Close();
    }
    if (Input::IsKeyDown(sf::Keyboard::Key::Space)) {
        renderWireframe = !renderWireframe;
    }

    camera.ProcessInput();
    
    // --- STEP 1: BIND OFFSCREEN BUFFER & SETUP PURE OPENGL STATE ---
    if (!gameRenderBuffer.setActive(true)) [[unlikely]] {
        std::cerr << "[-] Error: Failed to secure offscreen target handle focus\n";
        continue;
    }
    
    glViewport(0, 0, static_cast<GLsizei>(logicalWidth), static_cast<GLsizei>(logicalHeight));

    // FIX: Use native hardware clear calls instead of sf::RenderTexture::clear()
    // This prevents SFML's 2D context from silently overriding your depth test states
    glClearColor(0.117f, 0.129f, 0.176f, 1.0f); // Matching sf::Color(30, 33, 45) in normalized floats
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enforce clear, unambiguous 3D rendering states
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE); 
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND); // Turn off alpha blending during opaque voxel passes to guarantee depth writes

    if (renderWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // --- STEP 2: EXECUTE VOLUMETRIC GEOMETRY GENERATION AND DRAWS ---
    gameWorld.RebuildDirtyMeshes();

    renderer.BeginFrame(); 

    // Render the chunk buffers into the active, cleared offscreen texture context
    chunkRenderer.Render(gameWorld.GetRenderStates(), gameWorld.GetChunkPositions(), isoShader, camera);        
    
    // Finalize 3D rendering pass commands inside the texture target bounds
    gameRenderBuffer.display();

    // --- STEP 3: SWITCH CONTEXT TO THE PRIMARY WINDOW DEVICE ---
    if (!window.GetSFMLWindow().setActive(true)) [[unlikely]] {
        continue;
    }
    
    glViewport(0, 0, 1920, 1080);
    
    // Clear the main hardware viewport back buffer cleanly
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset state indicators to guarantee old loops don't leak shader logic pathways
    glUseProgram(0);                  
    glBindVertexArray(0);             
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Always draw the upscaled quad filled

    // --- STEP 4: UPSCALE DRAW PASS WITH SAFE STATE PRESERVATION ---
    // Protect the 3D state footprint by wrapping SFML's 2D draw operations cleanly
    window.GetSFMLWindow().pushGLStates();
    
    // Force texture coordinates adjustment or mapping adjustments if things are flipped vertically
    // If the image appears upside down, write: upscaleSprite.setScale({1.0f, -1.0f}); or handle here
    window.GetSFMLWindow().draw(upscaleSprite); 
    
    window.GetSFMLWindow().popGLStates();
    
    // Swap front and back hardware displays cleanly
    window.Display();
}

    return 0;
}