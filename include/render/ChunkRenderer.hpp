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
    ChunkRenderer() = default;
    ~ChunkRenderer() = default;

    void Render(const std::vector<ChunkRenderState>& renderStates,
                    const std::vector<glm::vec3>& chunkPositions,
                    const Shader& shader,
                    const Camera& camera) const noexcept;
    };
}