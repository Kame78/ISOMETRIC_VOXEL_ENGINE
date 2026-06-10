#pragma once

#include <vector>
#include <memory>
#include "Chunk.hpp"
#include "../render/ChunkMesher.hpp"
#include "../render/ChunkRenderer.hpp"

namespace World {

    class World{
    public:
        World() = default;
        ~World() = default;

        void GenerateDiorama(int widthChunks, int heightChunks, int depthChunks, uint32_t seed) noexcept;

        void RebuildDirtyMeshes() noexcept;

        [[nodiscard]] const std::vector<Chunk>& GetChunks() const noexcept {return m_chunks;}
        [[nodiscard]] std::vector<Chunk>& GetChunks() noexcept { return m_chunks; }

        [[nodiscard]] const std::vector<Render::ChunkRenderState>& GetRenderStates() const noexcept {return m_renderStates;}
        [[nodiscard]] std::vector<Render::ChunkRenderState>& GetRenderStates() noexcept {return m_renderStates;}

        [[nodiscard]] inline size_t GetChunkIndex(int cx, int cy, int cz) const noexcept {
            return static_cast<size_t>(cx + (cz * m_widthChunks) +(cy * m_widthChunks * m_depthChunks));
        }

    private:

        [[nodiscard]] const Chunk* GetChunkAt(int cx, int cy, int cz) const noexcept;
        [[nodiscard]] Chunk* GetChunkModifierAt(int cx, int cy, int cz) noexcept;

        int m_widthChunks = 0;
        int m_heightChunks = 0;
        int m_depthChunks = 0;

        std::vector<Chunk> m_chunks;
        std::vector<Render::ChunkRenderState> m_renderStates;
    };
}