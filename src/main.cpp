#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <ctime>

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

    // Load and populate the flight-weight block asset database from blocks.json
    World::BlockRegistry::Initialize();

    Renderer renderer;
    Render::ChunkRenderer chunkRenderer;
    Shader isoShader;

    if (!isoShader.LoadFromFile("assets/shaders/isometric.vert", "assets/shaders/isometric.frag")) {
        std::cerr << "CRITICAL: Shader compilation runtime fault detected!" << std::endl;
        return -1;
    }

    Camera camera(1920.0f, 1080.0f);

    uint32_t runtimeSeed = static_cast<uint32_t>(std::time(nullptr));
    std::cout << "WORLD PIPELINE INITIATED: Running Seed -> " << runtimeSeed << std::endl;

    World::World gameWorld;
    gameWorld.GenerateDiorama(12, 12, runtimeSeed);

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
        
        // === 5. GPU RENDERING DRAW PASS ===
        renderer.BeginFrame();

        if (renderWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Draw our whole vector block collection in one call tracking standard matrices
        chunkRenderer.Render(gameWorld.GetChunks(), gameWorld.GetRenderStates(), isoShader, camera);        
        
        window.Display();
    }

    return 0;
}