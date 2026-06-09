#include "../../include/world/World.hpp"
#include "../../include/world/WorldGenerator.hpp"
#include "../../include/world/FeatureDecorator.hpp"
#include <algorithm>

namespace World {

    void World::GenerateDiorama(int widthChunks, int depthChunks, uint32_t seed) noexcept {
        m_widthChunks = widthChunks;
        m_depthChunks = depthChunks;

        WorldGenerator generator;
        FeatureDecorator decorator;
        
        int totalChunks = m_widthChunks * m_depthChunks;

        m_chunks.clear();
        m_renderStates.clear();

        m_chunks.resize(totalChunks);
        m_renderStates.resize(totalChunks);

        const float totalWidthVoxels = static_cast<float>(m_widthChunks * CHUNK_SIZE);
        const float totalDepthVoxels = static_cast<float>(m_depthChunks * CHUNK_SIZE);
        const float centerX = totalWidthVoxels / 2.0f;
        const float centerZ = totalDepthVoxels / 2.0f;
        const float radius = (std::min(totalWidthVoxels, totalDepthVoxels) / 2.0f) - 1.0f;

        for (int cx = 0; cx < m_widthChunks; ++cx) {
            for (int cz = 0; cz < m_depthChunks; ++cz) {
                
                size_t index = static_cast<size_t>(cx + cz * m_widthChunks);

                Chunk chunk;
                chunk.worldPosition = glm::vec3(
                    static_cast<float>(cx * CHUNK_SIZE) * 2.f,
                    0.0f,
                    static_cast<float>(cz * CHUNK_SIZE) * 2.f
                );

                generator.GenerateChunk(chunk, cx, cz, centerX, centerZ, radius, seed);
                chunk.isDirty = true;

                m_chunks[index] = std::move(chunk);

            }
        }

        //decorator.DecorateWorld(m_chunks, seed);
    
        RebuildDirtyMeshes();
    
    }

    const Chunk* World::GetChunkAt(int cx, int cz) const noexcept {
        if (cx < 0 || cx >= m_widthChunks || cz < 0 || cz >= m_depthChunks) {
        return nullptr;
    }
    return &m_chunks[static_cast<size_t>(cx + cz * m_widthChunks)];
}

void World::RebuildDirtyMeshes() noexcept {
    if (m_chunks.empty()) return;

    for(int cx = 0; cx < m_widthChunks; ++cx) {
        for(int cz = 0; cz < m_depthChunks; ++cz) {
            size_t index = static_cast<size_t>(cx + cz * m_widthChunks);

            const Chunk* left = GetChunkAt(cx - 1, cz);
            const Chunk* right = GetChunkAt(cx + 1, cz);
            const Chunk* back = GetChunkAt(cx, cz -1);
            const Chunk* front = GetChunkAt(cx, cz + 1);

            Render::ChunkMesher::BuildMesh(
                m_chunks[index],
                m_renderStates[index].mesh,
                left, right, back, front
            );
            m_chunks[index].isDirty = false;
        }
    }
}

}