#include "../../include/world/WorldGenerator.hpp"
#include "../../include/math/NoiseMath.hpp"
#include <ranges>
#include <algorithm>
#include <iostream>

namespace World {

    StrataLayerPalette WorldGenerator::CompileDynamicPalette(const BlockRegistryTable& registry) noexcept {
        return StrataLayerPalette{
            .airID        = 0, // 0 is explicitly reserved for engine air spaces
            .surfaceID    = BlockOps::FindIdByGenerationLayer(registry, "surface"),
            .subsurfaceID = BlockOps::FindIdByGenerationLayer(registry, "subsurface"),
            .baseID       = BlockOps::FindIdByGenerationLayer(registry, "base")
        };
    }

    void WorldGenerator::GenerateChunk(
        Chunk& chunk, 
        int chunkX, 
        int chunkY, 
        int chunkZ, 
        float centerX, 
        float centerZ, 
        float radius, 
        uint32_t seed, 
        int totalWorldHeight,
        const StrataLayerPalette& layers) const noexcept 
    {
        const int worldOffsetX = chunkX * static_cast<int>(CHUNK_SIZE);
        const int worldOffsetY = chunkY * static_cast<int>(CHUNK_SIZE);
        const int worldOffsetZ = chunkZ * static_cast<int>(CHUNK_SIZE);

        auto voxelView = chunk.AsMdspan();
    
        // FIX: Core generation nesting realigned to match row-major voxel storage sequential rules (X -> Y -> Z)
        for (size_t x = 0; x < CHUNK_SIZE; ++x) {
            const float globalX = static_cast<float>(worldOffsetX + static_cast<int>(x)) * 2.0f;

            for (size_t y = 0; y < CHUNK_SIZE; ++y) {
                const int globalY = (chunkY * static_cast<int>(CHUNK_SIZE)) + static_cast<int>(y);

                for (size_t z = 0; z < CHUNK_SIZE; ++z) {
                    const float globalZ = static_cast<float>(worldOffsetZ + static_cast<int>(z)) * 2.0f;

                    // Evaluate noise configurations accurately per cell coordinate mapping context
                    const float landMask = Math::NoiseMath::CalculateBoundaryNoise(globalX, globalZ, centerX, centerZ, radius, seed);

                    if (landMask < 0.2f) {
                        voxelView[x, y, z] = layers.airID;
                        continue;
                    }

                    const float rawNoise = Math::NoiseMath::CalculateHeightNoise(globalX, globalZ, centerX, centerZ, radius, seed);
                    const int targetSurfaceY = Math::NoiseMath::QuantizeHeight(rawNoise, 3, totalWorldHeight);

                    if (globalY > targetSurfaceY) [[likely]] {
                        voxelView[x, y, z] = layers.airID;
                    }
                    else if (globalY == targetSurfaceY) {
                        voxelView[x, y, z] = layers.surfaceID;
                    }
                    else if (globalY < targetSurfaceY && globalY >= targetSurfaceY - 3) {
                        voxelView[x, y, z] = layers.subsurfaceID;
                    }
                    else {
                        voxelView[x, y, z] = layers.baseID;
                    }
                }
            }
        }
    }

} // namespace World