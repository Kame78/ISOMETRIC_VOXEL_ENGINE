#include <glad/glad.h> // 🔑 FIXED: Placed at the absolute top of the file to satisfy GLAD include constraints
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
    Window window;
    if (!window.Create(1920, 1080, "Production Architecture Voxel Workspace")) {
        std::cerr << "CRITICAL: Failed to construct primary window layer!" << std::endl;
        return -1;
    }

    const unsigned int logicalWidth  = 1920;
    const unsigned int logicalHeight = 1080;

    sf::ContextSettings textureSettings;
    textureSettings.depthBits = 24; 

    sf::RenderTexture gameRenderBuffer({logicalWidth, logicalHeight}, textureSettings);
    gameRenderBuffer.setSmooth(true); 

    // --- SFML 3 HIGH-PERFORMANCE UPSCALE CONTEXT ALIGNMENT ---
    sf::Sprite upscaleSprite(gameRenderBuffer.getTexture());
    upscaleSprite.setScale(sf::Vector2f(
        1920.f / static_cast<float>(logicalWidth),
        1080.f / static_cast<float>(logicalHeight)
    ));
    upscaleSprite.setOrigin(sf::Vector2f(0.0f, 0.0f));
    upscaleSprite.setPosition(sf::Vector2f(0.0f, 0.0f));

    auto blockRegistry = World::BlockOps::LoadRegistryFromFile("assets/data/blocks.json");

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
    gameWorld.GenerateDiorama(6, 3, 6, runtimeSeed, blockRegistry);

    float islandCenter = (6.0f * static_cast<float>(World::CHUNK_SIZE) * 2.0f) / 2.0f;
    camera.SetTarget(glm::vec3(islandCenter, 16.0f, islandCenter));

    bool renderWireframe = false;

    // 🔑 FIXED: Removed problematic glGetIntegerv block to ensure zero-fault compilation across platforms
    static bool useCulling = true;
    static bool useCCW     = true;

    gameWorld.RebuildDirtyMeshes();

    while (window.IsOpen()) {
        Input::UpdateStates();
        window.ProcessFrame();

        if (Input::IsKeyHeld(sf::Keyboard::Key::Escape)) {
            window.Close();
        }
        if (Input::IsKeyDown(sf::Keyboard::Key::Space)) {
            renderWireframe = !renderWireframe;
        }

        // ========================================================
        // LIVE RUNTIME DIAGNOSTIC KEYS (Use these to test inside-out behavior)
        // ========================================================
        if (Input::IsKeyDown(sf::Keyboard::Key::C)) {
            useCulling = !useCulling;
            std::cout << "[DIAGNOSTIC] Backface Culling toggled to: " << (useCulling ? "ENABLED" : "DISABLED") << "\n";
        }
        if (Input::IsKeyDown(sf::Keyboard::Key::V)) {
            useCCW = !useCCW;
            std::cout << "[DIAGNOSTIC] Winding Direction toggled to: " << (useCCW ? "GL_CCW (Standard)" : "GL_CW (Inverted)") << "\n";
        }
        // ========================================================

        camera.ProcessInput();
        
        // --- STEP 1: BIND OFFSCREEN BUFFER & SETUP PURE OPENGL STATE ---
        if (!gameRenderBuffer.setActive(true)) [[unlikely]] {
            std::cerr << "[-] Error: Failed to secure offscreen target handle focus\n";
            continue;
        }
        
        glViewport(0, 0, static_cast<GLsizei>(logicalWidth), static_cast<GLsizei>(logicalHeight));
        glClearColor(0.117f, 0.129f, 0.176f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); 
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND); 

        // Bind dynamic runtime live diagnostics
        if (useCulling) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }
        
        if (useCCW) {
            glFrontFace(GL_CCW);
        } else {
            glFrontFace(GL_CW); 
        }
        glCullFace(GL_BACK);

        if (renderWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // --- STEP 2: EXECUTE VOLUMETRIC GEOMETRY GENERATION AND DRAWS ---
        renderer.BeginFrame(); 
        chunkRenderer.Render(gameWorld.GetRenderStates(), gameWorld.GetChunkPositions(), isoShader, camera);        
        gameRenderBuffer.display();

        // --- STEP 3: SWITCH CONTEXT TO THE PRIMARY WINDOW DEVICE ---
        if (!window.GetSFMLWindow().setActive(true)) [[unlikely]] {
            continue;
        }
        
        glViewport(0, 0, 1920, 1080);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(0);                  
        glBindVertexArray(0);             
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 

        // --- STEP 4: UPSCALE CANVAS BLIT PASS WITH SFML 3 API ---
        window.GetSFMLWindow().pushGLStates();
        window.GetSFMLWindow().draw(upscaleSprite); 
        window.GetSFMLWindow().popGLStates();
        
        window.Display();
    }

    return 0;
}