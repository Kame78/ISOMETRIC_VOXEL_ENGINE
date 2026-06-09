#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <ctime>
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

    const unsigned int logicalWidth = 480;
    const unsigned int logicalHeight = 270;

    sf::ContextSettings textureSettings;
    textureSettings.depthBits = 24;


    sf::RenderTexture gameRenderBuffer({logicalWidth, logicalHeight}, textureSettings);

    gameRenderBuffer.setSmooth(false);

    sf::Sprite upscaleSprite(gameRenderBuffer.getTexture());
    upscaleSprite.setScale({
        1920.0f / static_cast<float>(logicalWidth),
        1080.0f / static_cast<float>(logicalHeight)
});

    // Load and populate the flight-weight block asset database from blocks.json
    World::BlockRegistry::Initialize();

    Renderer renderer;
    Render::ChunkRenderer chunkRenderer;
    Shader isoShader;

    if (!isoShader.LoadFromFile("assets/shaders/isometric.vert", "assets/shaders/isometric.frag")) {
        std::cerr << "CRITICAL: Shader compilation runtime fault detected!" << std::endl;
        return -1;
    }

    Camera camera(static_cast<float>(logicalWidth), static_cast<float>(logicalHeight));
    
    // === 2. ENCAPSULATED WORLD CONTAINER SETUP ===
    uint32_t runtimeSeed = static_cast<uint32_t>(std::time(nullptr));
    std::cout << "WORLD PIPELINE INITIATED: Running Seed -> " << runtimeSeed << std::endl;

    World::World gameWorld;
    gameWorld.GenerateDiorama(12, 12, runtimeSeed);

    // FIX: Dynamically calculate the exact center voxel of your 12x12 world grid
    // (12 chunks * CHUNK_SIZE voxels) / 2.0
    float islandCenter = (12.0f * static_cast<float>(World::CHUNK_SIZE)) / 2.0f;

    // Force the camera target to look directly at the center of the landmass, slightly elevated
    camera.SetTarget(glm::vec3(islandCenter, 16.0f, islandCenter));

    bool renderWireframe = false;

    // === 3. CORE RUNTIME APP LOOP ===

    while (window.IsOpen()) {
        // Poll OS framework events and refresh input buffers
        Input::UpdateStates();
        window.ProcessFrame();

        // Global key capture triggers
        if (Input::IsKeyHeld(sf::Keyboard::Key::Escape)) {
            window.Close();
        }

        if (Input::IsKeyDown(sf::Keyboard::Key::Space)) {
            renderWireframe = !renderWireframe;
        }

        // Camera transformation modifications
        camera.ProcessInput();
        
        gameWorld.RebuildDirtyMeshes();
        
       (void)gameRenderBuffer.setActive(true);
        glViewport(0, 0, static_cast<GLsizei>(logicalWidth), static_cast<GLsizei>(logicalHeight));

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        renderer.BeginFrame();

        if (renderWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        chunkRenderer.Render(gameWorld.GetChunks(), gameWorld.GetRenderStates(), isoShader, camera);        
        
        gameRenderBuffer.display();

        // -----------------------------------------------------------------
        // PASS B: Blit and Upscale the Texture onto the Native Monitor Screen
        // -----------------------------------------------------------------
        // FIX: Cast to void to satisfy the nodiscard attribute rule here as well
        (void)window.GetSFMLWindow().setActive(true);
        glViewport(0, 0, 1920, 1080);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        window.GetSFMLWindow().pushGLStates();
        window.GetSFMLWindow().draw(upscaleSprite);
        window.GetSFMLWindow().popGLStates();
        
        window.Display();
    }

    return 0;
}