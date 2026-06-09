#include "../../include/world/FeatureDecorator.hpp"
#include "../../include/math/NoiseMath.hpp"
#include <cmath>


namespace World {

namespace {
    float GetFeatureHash(int globalX, int globalZ, uint32_t seed) noexcept {
        int n = globalX * 131 + globalZ * 41 + static_cast<int> (seed * 101);
        n = (n << 13) ^ n;
        int hashed = (n * (n * n * 19333 + 811603) + 1225547) & 0x7fffffff;
        return static_cast<float>(hashed % 1000) / 1000.0f;
    }
}

bool FeatureDecorator::IsAreaFlat(const Chunk& chunk, int startX, int startY, int startZ, int width, int depth) const noexcept {
    for (int x = startX; x < startX + width; ++x) {
        for (int z = startZ; z < startZ + depth; ++z) {
            if (chunk.GetBlock(x, startY, z) !=BlockID::GRASS) {
                return false;
            }
        }
    }
    return true;
}

void FeatureDecorator::DecorateWorld(std::vector<Chunk>& chunks, uint32_t seed) const noexcept {
    for(auto& chunk : chunks) {
        int worldOffsetX = static_cast<int>(chunk.worldPosition.x);
        int worldOffsetZ = static_cast<int>(chunk.worldPosition.z);

        for (int x = 2; x < CHUNK_SIZE - 2; ++x) {
            for (int z = 2; z < CHUNK_SIZE - 2; ++z) {

                int globalX = worldOffsetX + x;
                int globalZ = worldOffsetZ + z;

                float rockDensity = Math::NoiseMath::CalculateRockDensity(static_cast<float>(globalX), static_cast<float>(globalZ), seed);

                if (rockDensity < 0.22f) continue;

                float spawnRoll = GetFeatureHash(globalX, globalZ, seed);

                if (spawnRoll > 0.985f) {
                    for (int y = CHUNK_SIZE - 1; y >= 0; --y) {
                        if (chunk.GetBlock(x, y, z) == BlockID::GRASS) {

                            if(IsAreaFlat(chunk, x -2, y, z - 2, 5, 5)) {
                                SpawnRockOutcrop(chunk, x, y, z);
                                x += 4;
                                z += 4;
                            }
                            break;
                        }
                    }
                }
            }
        }
        chunk.isDirty = true;
    }
}

void FeatureDecorator::SpawnRockOutcrop(Chunk& chunk, int tx, int ty, int tz) const noexcept {
    for (int dx = -2; dx <= 2; ++ dx) {
        for (int dz = -2; dz <= 2; ++dz) {
            chunk.SetBlock(tx + dx, ty,     tz + dz, BlockID::STONE);
            chunk.SetBlock(tx + dx, ty + 1, tz + dz, BlockID::STONE);
        }
    }
    for (int dx = -2; dx <= 2; ++dx) {
        for (int dz = -2; dz <= 2; ++dz) {
            if(std::abs(dx) <= 1 && std::abs(dz) <= 1) {
                chunk.SetBlock(tx + dx, ty + 2, tz + dz, BlockID::STONE);
            } else {
                chunk.SetBlock(tx + dx, ty + 2, tz + dz, BlockID::GRASS);    
            }
        }
    }

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dz = -1; dz <= 1; ++dz) {
            chunk.SetBlock(tx + dx, ty + 3, tz + dz, BlockID::STONE);
            chunk.SetBlock(tx + dx, ty + 4, tz + dz, BlockID::GRASS);
        }
    }
}
    
}