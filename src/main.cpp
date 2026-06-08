#include <iostream>
#include <glm/glm.hpp>

#include "core/Window.hpp"
#include "core/Input.hpp"
#include "render/Shader.hpp"
#include "render/Camera.hpp"
#include "render/Renderer.hpp"
#include "render/ChunkRenderer.hpp"
#include "world/Chunk.hpp"
#include "world/ChunkBuilder.hpp"
#include "render/Mesh.hpp"
#include "render/Primitives.hpp"
#include "render/Renderer.hpp"

int main() {

    // Initialization

    Window window;
    if (!window.Create(1280, 720, "Production Architecture Voxel Workspace")) return -1;

    Renderer renderer;
    Render::ChunkRenderer chunkRenderer;
    Shader isoShader;

    if (!isoShader.LoadFromFile("assets/shaders/isometric.vert", "assets/shaders/isometric.frag")) {
        return -1;
    }

    Camera camera(1280.0f, 720.0f);

    // WORLD SETUP

    std::vector<World::Chunk> worldChunks;
    std::vector<Render::ChunkRenderState> renderStates;

    // Create One Test Chunk

    World::Chunk myChunk;
    myChunk.worldPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    for(int x = 0; x < 16; ++x) {
        for(int y = 0; y < 16; ++y) {
            for(int z = 0; z < 16; ++z) {
                myChunk.SetBlock(x, y, z, World::BlockID::STONE);
            }
        }
    }
    // Mark as dirty so it builds the mesh exactly once when the loop starts
    myChunk.isDirty = true;

    // Generate terrain logic
    worldChunks.push_back(myChunk);
    renderStates.emplace_back(); // Initalize default mesh state

    bool renderWireframe = false;

    while (window.IsOpen()) {
        Input::UpdateStates();
        window.ProcessFrame();

        if (Input::IsKeyHeld(sf::Keyboard::Key::Escape)) window.Close();
        camera.ProcessInput();

        if (Input::IsKeyDown(sf::Keyboard::Key::Space)) renderWireframe = !renderWireframe;
        camera.ProcessInput();

        for ( size_t i = 0; i < worldChunks.size(); ++i) {
            if (worldChunks[i].isDirty) {
                World::ChunkBuilder::BuildMesh(worldChunks[i], renderStates[i].mesh);
                worldChunks[i].isDirty = false;
            }
        }

        renderer.BeginFrame();

        if (renderWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        chunkRenderer.Render(worldChunks, renderStates, isoShader, camera);
        window.Display();
    }

    return 0;
}