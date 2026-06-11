#include "../../include/world/World.hpp"
#include "../../include/world/WorldGenerator.hpp"
#include "../../include/world/FeatureDecorator.hpp"
#include <algorithm>

namespace World {

    void World::GenerateDiorama(int widthChunks, int heightChunks, int depthChunks, uint32_t seed, const BlockRegistryTable& registry) noexcept {
        m_widthChunks = widthChunks;
        m_heightChunks = heightChunks;
        m_depthChunks = depthChunks;

        WorldGenerator generator;
        int totalChunks = m_widthChunks * m_heightChunks * m_depthChunks;

        m_chunks.clear();
        m_renderStates.clear();
        m_worldPositions.clear();
        m_dirtyFlags.clear();


        m_chunks.resize(totalChunks);
        m_renderStates.resize(totalChunks);
        m_worldPositions.resize(totalChunks);
        m_dirtyFlags.resize(totalChunks, 1);

        const float totalWidthWorld = static_cast<float>(m_widthChunks * CHUNK_SIZE) * 2.0f;
        const float totalDepthWorld = static_cast<float>(m_depthChunks * CHUNK_SIZE) * 2.0f;
        
        const float centerX = totalWidthWorld / 2.0f;
        const float centerZ = totalDepthWorld / 2.0f;
        const float radius  = (std::min(totalWidthWorld, totalDepthWorld) / 2.0f) - 4.0f;

        // Build across the three dimensional boundaries
        int totalWorldHeight = m_heightChunks * CHUNK_SIZE;

        auto dynamicPalette = WorldGenerator::CompileDynamicPalette(registry);

        for (int cy = 0; cy < m_widthChunks; ++cy) {
            for (int cz = 0; cz < m_heightChunks; ++cz) {
                for (int cx = 0; cx < m_depthChunks; ++cx) {
                    
                    size_t index = GetChunkIndex(cx, cy, cz);

                    m_worldPositions[index] = glm::vec3(
                        static_cast<float>(cx * CHUNK_SIZE) * 2.0f,
                        static_cast<float>(cy * CHUNK_SIZE) * 1.0f,
                        static_cast<float>(cz * CHUNK_SIZE) * 2.0f
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

    for (int cy = 0; cy < m_widthChunks; ++cy) {
        for (int cz = 0; cz < m_heightChunks; ++cz) {
            for (int cx = 0; cx < m_depthChunks; ++cx) {
                size_t index = GetChunkIndex(cx, cy, cz);

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
                    left, right, back, front, bottom, top
                );
                m_chunks[index].isDirty = 0;
            }
        }
    }
}

} // namespace World