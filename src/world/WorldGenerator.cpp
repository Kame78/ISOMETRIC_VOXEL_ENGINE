#include "../../include/world/WorldGenerator.hpp"
#include "../../include/math/NoiseMath.hpp"
#include <ranges>
#include <algorithm>

namespace World {

void WorldGenerator::GenerateChunk(Chunk& chunk, int chunkX, int chunkZ, float centerX, float centerZ, float radius, uint32_t seed) const noexcept {
    int worldOffsetX = chunkX * CHUNK_SIZE;
    int worldOffsetZ = chunkZ * CHUNK_SIZE;

    auto columns = std::views::cartesian_product(
        std::views::iota(0, CHUNK_SIZE),
        std::views::iota(0, CHUNK_SIZE)
    );
        
    for (auto [x, z] : columns) {
        float globalX = static_cast<float>(worldOffsetX + x);
        float globalZ = static_cast<float>(worldOffsetZ + z);

        float landMask = Math::NoiseMath::CalculateBoundaryNoise(globalX, globalZ, centerX, centerZ, radius, seed);
            
        if (landMask < -0.2f) {
            for( int y = 0; y < CHUNK_SIZE; ++ y) {
                chunk.SetBlock(x, y, z, BlockID::AIR);
            }
            continue;
        }

        float rawNoise = Math::NoiseMath::CalculateHeightNoise(globalX, globalZ, centerX, centerZ, radius, seed);
        int targetSurfaceY = Math::NoiseMath::QuantizeHeight(rawNoise, 3);
        targetSurfaceY = std::clamp(targetSurfaceY, 0, CHUNK_SIZE - 1);

        for (int y = CHUNK_SIZE - 1; y >= 0; --y){
            if(y > targetSurfaceY) {
                chunk.SetBlock(x, y, z, BlockID::AIR);
            }
            else if (y == targetSurfaceY) {
                chunk.SetBlock(x, y, z, BlockID::GRASS);
            }
            else if (y < targetSurfaceY && y>= targetSurfaceY - 3) {
                chunk.SetBlock(x, y, z, BlockID::DIRT);
            }
            else if (y < targetSurfaceY - 3 && y>= targetSurfaceY - 6) {
                chunk.SetBlock(x, y, z, BlockID::STONE);
            }
            else {
                chunk.SetBlock(x, y, z, BlockID::AIR);
            }
        }
    }
}

}