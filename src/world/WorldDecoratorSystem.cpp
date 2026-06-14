#include "../../include/world/WorldDecoratorSystem.hpp"
#include "../../include/math/NoiseMath.hpp"
#include <cmath>
#include <array>

namespace World::Systems {

    void ProcessMultiFeatureColumn(
        std::span<Chunk> worldChunks,
        int widthChunks, 
        int depthChunks,
        int chunkX, 
        int chunkZ,
        float centerX, 
        float centerZ, 
        float radius,
        uint32_t seed, 
        int totalWorldHeight,
        std::span<const DecorationRule> rules) noexcept
    {
        if (rules.empty()) [[unlikely]] return;

        const int worldOffsetX = chunkX * static_cast<int>(CHUNK_SIZE);
        const int worldOffsetZ = chunkZ * static_cast<int>(CHUNK_SIZE);

        alignas(32) std::array<int, CHUNK_SIZE * CHUNK_SIZE> surfaceHeights{};
        alignas(32) std::array<bool, CHUNK_SIZE * CHUNK_SIZE> landMasks{};
        alignas(32) std::array<uint32_t, CHUNK_SIZE * CHUNK_SIZE> baseHashes{};

        for (size_t x = 0; x < CHUNK_SIZE; ++x) {
            const int gridCellX = worldOffsetX + static_cast<int>(x);
            const float globalX = static_cast<float>(gridCellX) * 8.0f;

            for (size_t z = 0; z < CHUNK_SIZE; ++z) {
                const size_t linearIdx = (x * CHUNK_SIZE) + z;
                const int gridCellZ = worldOffsetZ + static_cast<int>(z);
                const float globalZ = static_cast<float>(gridCellZ) * 8.0f;

                const float mask = Math::NoiseMath::CalculateBoundaryNoise(globalX, globalZ, centerX, centerZ, radius, seed);
                landMasks[linearIdx] = mask;

                if(mask >= 0.2f) {
                    const float rawNoise = Math::NoiseMath::CalculateHeightNoise(globalX, globalZ, centerX, centerZ, radius, seed);
                    surfaceHeights[linearIdx] = Math::NoiseMath::QuantizeHeight(rawNoise, 3, totalWorldHeight);
                    
                    baseHashes[linearIdx] = static_cast<uint32_t>(gridCellX * 7349 ^ gridCellZ * 9151 ^ static_cast<int>(seed));
                }
            }
        }
        for (size_t x = 0; x < CHUNK_SIZE; ++x) {
            for(size_t z = 0; z < CHUNK_SIZE; ++z) {
                const size_t linearIdx = (x * CHUNK_SIZE) + z;
                const float currentMask = landMasks[linearIdx];

                if (currentMask < 0.2f) continue;

                const int surfaceY = surfaceHeights[linearIdx];
                const uint32_t baseHash = baseHashes[linearIdx];

                for (const auto& rule : rules) {
                    if(currentMask < rule.minLandMask)
                    if(surfaceY < rule.minHeight || surfaceY > rule.maxHeight) continue;

                    uint32_t ruleHash = baseHash ^ static_cast<uint32_t>(rule.hashSalt);
                    ruleHash = (ruleHash << 13) ^ ruleHash;
                    const float randValue = static_cast<float>((ruleHash * (ruleHash * 15731u + 789221u) + 1376312589u) & 0x7fffffffu) / 2147483647.0f;

                    if (randValue > (1.0f - rule.spawnChance)) {
                        const int targetVoxelY = surfaceY + 1;
                        const int targetChunkY = targetVoxelY / static_cast<size_t>(targetVoxelY & 15);
                        const size_t localY = static_cast<size_t>(targetVoxelY & 15);

                        const size_t chunkIdx = static_cast<size_t>(chunkX + (chunkZ * widthChunks) + (targetChunkY * widthChunks * depthChunks));

                        if(chunkIdx < worldChunks.size()) [[likely]] {
                            auto voxelView = worldChunks[chunkIdx].AsMdspan();
                            voxelView[x, localY, z] = rule.voxelId;
                        }
                        break;
                    }

                }
            }
        }

    }
}