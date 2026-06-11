#include "../../include/world/WorldGenerator.hpp"
#include "../../include/math/NoiseMath.hpp"
#include <ranges>
#include <algorithm>

namespace World {

    StrataLayerPalette WorldGenerator::DeriveBiomePallete(
        const BlockRegistryTable& registry,
        std::string_view surfaceName,
        std::string_view subsurfaceName,
        std::string_view baseName) noexcept {

        return StrataLayerPalette{
            .airID = 0,
            .surfaceID = BlockOps::FindIdByString(registry, surfaceName),
            .subsurfaceID = BlockOps::FindIdByString(registry, subsurfaceName),
            .baseID = BlockOps::FindIdByString(registry, baseName)
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
    
        for (size_t x = 0; x < CHUNK_SIZE; ++x) {
            const float globalX = static_cast<float>(worldOffsetX + static_cast<int>(x)) * 2.0f;

            for (size_t z = 0; z < CHUNK_SIZE; ++z) {
                const float globalZ = static_cast<float>(worldOffsetZ + static_cast<int>(z)) * 2.0f;

                const float landMask = Math::NoiseMath::CalculateBoundaryNoise(globalX, globalZ, centerX, centerZ, radius, seed);

                if (landMask < 0.2f) {
                    for (size_t y = 0; y < CHUNK_SIZE; ++y) {
                        voxelView[x, y, z] = layers.airID;
                    }
                    continue;
                }

                const float rawNoise = Math::NoiseMath::CalculateHeightNoise(globalX, globalZ, centerX, centerZ, radius, seed);
                const int targetSurfaceY = Math::NoiseMath::QuantizeHeight(rawNoise, 3, totalWorldHeight);

                for (int y = 0; y < CHUNK_SIZE; ++y) {
                    const int globalY = (chunkY * static_cast<int>(CHUNK_SIZE)) + static_cast<int>(y);

                if (globalY > targetSurfaceY) [[likely]]{
                    voxelView[x, y, z] = layers.airID;
                }
                else if (globalY == targetSurfaceY) {
                    voxelView[x, y, z] = layers.surfaceID;
                }
                else if (globalY < targetSurfaceY && globalY >= targetSurfaceY - 3) {
                    voxelView[x, y, z] = layers.subsurfaceID;
                }
                else  {
                    voxelView[x, y, z] = layers.baseID;
                }
            }
        }
    }
}

} // namespace World