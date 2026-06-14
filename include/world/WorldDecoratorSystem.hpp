#pragma once
#include "Chunk.hpp"
#include "BlockRegistry.hpp"
#include <cstdint>

namespace World::Systems {

    struct alignas(16) DecorationRule {
        VoxelTypeID voxelId;
        int16_t minHeight;
        int16_t maxHeight;
        float minLandMask;
        float spawnChance;
        uint16_t hashSalt;
    };

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
        std::span<const DecorationRule> rules) noexcept;
    }

