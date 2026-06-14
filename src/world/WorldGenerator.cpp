#include "../../include/world/WorldGenerator.hpp"
#include "../../include/math/NoiseMath.hpp"
#include <ranges>
#include <algorithm>
#include <iostream>

namespace World {

    void WorldGenerator::GenerateChunk(
        Chunk& outChunk, 
        int cx, 
        int cy, 
        int cz, 
        float centerX, 
        float centerZ, 
        float radius, 
        uint32_t seed, 
        int totalWorldHeight,
        const StrataLayerPalette& palette) const noexcept 
    {
        auto voxelView = outChunk.AsMdspan();
        const int chunkBaseY = cy * static_cast<int>(CHUNK_SIZE);

      // 🔑 CORRECT LAYOUT SEQUENCE: X -> Z -> Y
        for (size_t x = 0; x < CHUNK_SIZE; ++x) {
            const float globalX = static_cast<float>(cx * static_cast<int>(CHUNK_SIZE) + static_cast<int>(x)) * 8.0f;

            for (size_t y = 0; y < CHUNK_SIZE; ++y) {
                const int globalY = chunkBaseY + static_cast<int>(y);

            for (size_t z = 0; z < CHUNK_SIZE; ++z) {
                const float globalZ = static_cast<float>(cz * static_cast<int>(CHUNK_SIZE) + static_cast<int>(z)) * 8.0f;

                // Evaluate noise coordinates ONCE per single horizontal column map step
                const float landMask = Math::NoiseMath::CalculateBoundaryNoise(globalX, globalZ, centerX, centerZ, radius, seed);

                if (landMask < 0.2f) {
                    if(globalY <= 4) {
                        voxelView[x, y, z] = palette.waterID;
                    } else {
                        voxelView[x, y, z] = palette.airID;
                    }
                    continue;
                }

                const float rawNoise = Math::NoiseMath::CalculateHeightNoise(globalX, globalZ, centerX, centerZ, radius, seed);
                const int targetSurfaceY = Math::NoiseMath::QuantizeHeight(rawNoise, 3, totalWorldHeight);

              // Map the matching VoxelTypeID tokens onto our vertical column slice
                    if (globalY > targetSurfaceY) {
                        voxelView[x, y, z] = palette.airID;
                    } else if (globalY == targetSurfaceY) {
                        voxelView[x, y, z] = palette.surfaceID;
                    } else if (globalY >= targetSurfaceY - 4) {
                        voxelView[x, y, z] = palette.subsurfaceID;
                    } else {
                        voxelView[x, y, z] = palette.baseID;
                    }
                }
            }
        }
    }
}