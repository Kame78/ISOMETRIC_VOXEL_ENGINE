#include "../../include/world/World.hpp"
#include "../../include/world/WorldGenerator.hpp"
#include <algorithm>
#include <iostream>

namespace World {

    constexpr float BLOCK_WIDTH = 8.0f;
    constexpr float BLOCK_HEIGHT = 1.0f;
    constexpr float BLOCK_DEPTH = 8.0f;

    void World::GenerateDiorama(int widthChunks, int heightChunks, int depthChunks, uint32_t seed, const BlockRegistryTable& registry) noexcept {
        m_registryReference = &registry;
        
        m_widthChunks = widthChunks;
        m_heightChunks = heightChunks;
        m_depthChunks = depthChunks;

        WorldGenerator generator;
        size_t totalChunks = static_cast<size_t>(widthChunks) * static_cast<size_t>(heightChunks) * static_cast<size_t>(depthChunks);

        m_chunks.clear();
        m_renderStates.clear();
        m_worldPositions.clear();
        m_dirtyFlags.clear();


        m_chunks.assign(totalChunks, Chunk());
        m_renderStates.resize(totalChunks);
        m_worldPositions.resize(totalChunks);
        m_dirtyFlags.assign(totalChunks, 1);

        const float totalWidthWorld = static_cast<float>(m_widthChunks * CHUNK_SIZE) * 8;
        const float totalDepthWorld = static_cast<float>(m_depthChunks * CHUNK_SIZE) * 8;
        
        const float centerX = totalWidthWorld / 2.0f;
        const float centerZ = totalDepthWorld / 2.0f;
        const float radius  = (std::min(totalWidthWorld, totalDepthWorld) / 2.0f) - 4.0f;

        // Build across the three dimensional boundaries
        int totalWorldHeight = m_heightChunks * CHUNK_SIZE;

        auto dynamicPalette = WorldGenerator::CompileDynamicPalette(registry);

        for (int cy = 0; cy < m_heightChunks; ++cy) {
            for (int cz = 0; cz < m_depthChunks; ++cz) {
                for (int cx = 0; cx < m_widthChunks; ++cx) {
                    
                    const size_t index = GetChunkIndex(cx, cy, cz);

                    m_worldPositions[index] = glm::vec3(
                      static_cast<float>(cx) * (static_cast<float>(CHUNK_SIZE) * BLOCK_WIDTH),
                    static_cast<float>(cy) * (static_cast<float>(CHUNK_SIZE) * BLOCK_HEIGHT),
                    static_cast<float>(cz) * (static_cast<float>(CHUNK_SIZE) * BLOCK_DEPTH)
                    );

                    // Pass chunkY down so the generation logic tracks its stack height
                    generator.GenerateChunk(m_chunks[index], cx, cy, cz, centerX, centerZ, radius, seed, totalWorldHeight, dynamicPalette);
                    m_dirtyFlags[index] = 1;
                }
            }
        }
    
        RebuildDirtyMeshes();
    }

    const Chunk* World::GetChunkAt(int cx, int cy, int cz) const noexcept {
        if (cx < 0 || cx >= m_widthChunks || cy < 0 || cy >= m_heightChunks || cz < 0 || cz >= m_depthChunks) [[unlikely]] {
            return nullptr;
        }
        return &m_chunks[GetChunkIndex(cx, cy, cz)];
    }

    Chunk* World::GetChunkModifierAt(int cx, int cy, int cz) noexcept {
        if (cx < 0 || cx >= m_widthChunks || cy < 0 || cy >= m_heightChunks || cz < 0 || cz >= m_depthChunks) [[unlikely]] {
            return nullptr;
        }
        return &m_chunks[GetChunkIndex(cx, cy, cz)];
    }

void World::RebuildDirtyMeshes() noexcept {
    if (m_chunks.empty()) return;

    for (int cy = 0; cy < m_heightChunks; ++cy) {
        for (int cz = 0; cz < m_depthChunks; ++cz) {
            for (int cx = 0; cx < m_widthChunks; ++cx) {
                const size_t index = GetChunkIndex(cx, cy, cz);

                if (m_dirtyFlags[index] == 0) [[likely]] {
                    continue;
                }

                const Chunk* left   = GetChunkAt(cx - 1, cy, cz);
                const Chunk* right  = GetChunkAt(cx + 1, cy, cz);
                const Chunk* back   = GetChunkAt(cx, cy, cz - 1);
                const Chunk* front  = GetChunkAt(cx, cy, cz + 1);
                const Chunk* bottom = GetChunkAt(cx, cy - 1, cz);
                const Chunk* top    = GetChunkAt(cx, cy + 1, cz);

                Render::ChunkMesher::BuildMesh(
                    m_chunks[index],
                    m_renderStates[index].mesh,
                    left, right, back, front, bottom, top,
                    *m_registryReference
                );
               
                 m_dirtyFlags[index] = 0;

            }
        }
    }
}

} // namespace World