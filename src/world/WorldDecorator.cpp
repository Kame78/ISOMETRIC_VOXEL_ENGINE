#include "../../include/world/WorldDecorator.hpp"
#include "../../include/math/NoiseMath.hpp"
#include <cmath>
#include <algorithm>

namespace World {

    void WorldDecorator::DecorateChunk(
        Chunk& chunk,
        int chunkX,
        int chunkY,
        int chunkZ,
        float centerX,
        float centerZ,
        float radius,
        uint32_t seed,
        int totalWorldHeight,
        const BlockRegistryTable& registry) const noexcept
    {
        const int worldOffsetX = chunkX * static_cast<int>(CHUNK_SIZE);
        const int worldOffsetZ = chunkZ * static_cast<int>(CHUNK_SIZE);

        auto voxelView = chunk.AsMdspan();

        const VoxelTypeID treeSpriteID = BlockOps::FindIdByString(registry, "engine:tree");
        if(treeSpriteID == 0) return;

        const int MOUNTAIN_THRESHOLD_Y = 39;

        for (size_t x = 0; x < CHUNK_SIZE; ++x) {
            const float globalX = static_cast<float>(worldOffsetX + static_cast<int>(x)) * 8.0f;

            for (size_t z = 0; z < CHUNK_SIZE; ++z) {
                const float globalZ = static_cast<float>(worldOffsetZ + static_cast<int>(z)) * 8.0f;

                const float landMask = Math::NoiseMath::CalculateBoundaryNoise(globalX, globalZ, centerX, centerZ, radius, seed);
                if (landMask < 0.2f) continue;

                const float rawNoise = Math::NoiseMath::CalculateHeightNoise(globalX, globalZ, centerX, centerZ, radius, seed);
                const int targetSurfaceY = Math::NoiseMath::QuantizeHeight(rawNoise, 3, totalWorldHeight);

                if (targetSurfaceY >= MOUNTAIN_THRESHOLD_Y || targetSurfaceY <= 3) continue;

                int gridCellX = static_cast<int>(std::floor(std::floor(globalX / 8.0f)));
                int gridCellZ = static_cast<int>(std::floor(std::floor(globalZ / 8.0f)));
                uint32_t cellHash = static_cast<uint32_t>(gridCellX * 7349 ^ gridCellZ * 9151 ^ static_cast<int>(seed));
                cellHash = (cellHash << 13) ^ cellHash;
                float treeChance = static_cast<float>((cellHash * (cellHash * 15731u + 789221u) + 1376312589u) & 0x7fffffffu) / 2147483647.0f;

                if (treeChance > 0.982f) {
                    int targetVoxelY = targetSurfaceY + 1;

                    int localY = targetVoxelY - (chunkY * static_cast<int>(CHUNK_SIZE));
                    if (localY >= 0 && localY < static_cast<int>(CHUNK_SIZE)) {
                        voxelView[x, static_cast<size_t>(localY), z] = treeSpriteID;
                    }
                }
            }
        }
    }

}
