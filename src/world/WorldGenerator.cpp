#include "../../include/world/WorldGenerator.hpp"
#include "../../include/math/NoiseMath.hpp"
#include <ranges>
#include <algorithm>

namespace World {

void WorldGenerator::GenerateChunk(Chunk& chunk, int chunkX, int chunkY, int chunkZ, float centerX, float centerZ, float radius, uint32_t seed, int totalWorldHeight) const noexcept {
    int worldOffsetX = chunkX * CHUNK_SIZE;
    int worldOffsetZ = chunkZ * CHUNK_SIZE;

    auto columns = std::views::cartesian_product(
        std::views::iota(0, CHUNK_SIZE),
        std::views::iota(0, CHUNK_SIZE)
    );
        
    for (auto [x, z] : columns) {
        float globalX = static_cast<float>(worldOffsetX + x) * 2.f;
        float globalZ = static_cast<float>(worldOffsetZ + z) * 2.f;

        float landMask = Math::NoiseMath::CalculateBoundaryNoise(globalX, globalZ, centerX, centerZ, radius, seed);
            
        if (landMask < -0.2f) {
            for( int y = 0; y < CHUNK_SIZE; ++ y) {
                chunk.SetBlock(x, y, z, BlockID::AIR);
            }
            continue;
        }

        float rawNoise = Math::NoiseMath::CalculateHeightNoise(globalX, globalZ, centerX, centerZ, radius, seed);
       
        int targetSurfaceY = Math::NoiseMath::QuantizeHeight(rawNoise, 3, totalWorldHeight);

        for (int y = CHUNK_SIZE - 1; y >= 0; --y) {
            // Compute exactly where this specific block sits globally in the vertical stack
            int globalY = (chunkY * CHUNK_SIZE) + y;

            if (globalY > targetSurfaceY) {
                chunk.SetBlock(x, y, z, BlockID::AIR);
            }
            else if (globalY == targetSurfaceY) {
                chunk.SetBlock(x, y, z, BlockID::GRASS);
            }
            else if (globalY < targetSurfaceY && globalY >= targetSurfaceY - 3) {
                chunk.SetBlock(x, y, z, BlockID::DIRT);
            }
            else if (globalY < targetSurfaceY - 3) {
                // Solid core foundation filling down through underlying chunk segments
                chunk.SetBlock(x, y, z, BlockID::STONE);
            }
            else [[unlikely]] {
                chunk.SetBlock(x, y, z, BlockID::AIR);
            }
        }
    }
}

} // namespace World