#pragma once
#include "Chunk.hpp"
#include "BlockRegistry.hpp"
#include <cstdint>

namespace World {
class WorldDecorator {
public:

    void DecorateChunk(
        Chunk& chunk,
        int chunkX,
        int chunkY,
        int chunkZ,
        float centerX,
        float centerZ,
        float radius,
        uint32_t seed,
        int totalWorldHeight,
        const BlockRegistryTable& registry) const noexcept;
    };

}
