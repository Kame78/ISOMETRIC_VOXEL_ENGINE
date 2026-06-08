#pragma once
#include "../world/Chunk.hpp"
#include "Renderer.hpp"
#include <vector>

namespace Render {
    struct ChunkRenderState {
        Mesh mesh;
        bool isVisible{ true};
    };

    class ChunkRenderer {
    public:

    void Render(const std::vector<World::Chunk>& chunks,
                const std::vector<ChunkRenderState>& renderStates,
                const Shader& shader,
                const Camera& camera) const;
    };
}