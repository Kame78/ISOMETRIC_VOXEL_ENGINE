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
            .baseID       = BlockOps::FindIdByGenerationLayer(registry, "base"),
            .waterID      = BlockOps::FindIdByGenerationLayer(registry, "engine:water")
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
        const int worldOffsetZ = chunkZ * static_cast<int>(CHUNK_SIZE);

        auto voxelView = chunk.AsMdspan();
        const int MOUNTAIN_THRESHOLD_Y = 39;

        const int SEA_LEVEL_Y = 33;

      // 🔑 CORRECT LAYOUT SEQUENCE: X -> Z -> Y
        for (size_t x = 0; x < CHUNK_SIZE; ++x) {
            const float globalX = static_cast<float>(worldOffsetX + static_cast<int>(x)) * 8.0f;

            for (size_t z = 0; z < CHUNK_SIZE; ++z) {
                const float globalZ = static_cast<float>(worldOffsetZ + static_cast<int>(z)) * 8.0f;

                // Evaluate noise coordinates ONCE per single horizontal column map step
                const float landMask = Math::NoiseMath::CalculateBoundaryNoise(globalX, globalZ, centerX, centerZ, radius, seed);
                const float rawNoise = Math::NoiseMath::CalculateHeightNoise(globalX, globalZ, centerX, centerZ, radius, seed);
                const int targetSurfaceY = Math::NoiseMath::QuantizeHeight(rawNoise, 3, totalWorldHeight);

                for (size_t y = 0; y < CHUNK_SIZE; ++y) {
                    const int globalY = (chunkY * static_cast<int>(CHUNK_SIZE)) + static_cast<int>(y);

                    // =========================================================
                    // ZONE A: OPEN OCEAN (Outside the island perimeter mask)
                    // =========================================================
                    if (landMask < 0.2f) {
                        if (globalY <= SEA_LEVEL_Y) {
                            voxelView[x, y, z] = layers.waterID; 
                        } else {
                            voxelView[x, y, z] = layers.airID;   
                        }
                        continue;
                    }

                    // =========================================================
                    // ZONE B: THE ISLAND LANDMASS
                    // =========================================================
                    if (globalY > targetSurfaceY) {
                        if (globalY <= SEA_LEVEL_Y) {
                            voxelView[x, y, z] = layers.waterID;
                        } else {
                            voxelView[x, y, z] = layers.airID;
                        }
                    }
                    else if (targetSurfaceY >= MOUNTAIN_THRESHOLD_Y) {
                        voxelView[x, y, z] = layers.baseID; // Bare stone peaks
                    }
                    else {
                        // Lowland strata configurations
                        if (globalY == targetSurfaceY) {
                            voxelView[x, y, z] = layers.surfaceID; 
                        }
                        else if (globalY < targetSurfaceY && globalY >= targetSurfaceY - 5) {
                            voxelView[x, y, z] = layers.subsurfaceID; 
                        }
                        else {
                            voxelView[x, y, z] = layers.baseID; 
                        }
                    }
                } // End of y loop
            } // End of z loop
        } // End of x loop
    }

} // namespace World