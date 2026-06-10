#pragma once

#include "Chunk.hpp"

namespace World {

    class WorldGenerator {
    public:

        WorldGenerator() = default;
        ~WorldGenerator() = default;

        WorldGenerator(const WorldGenerator&) = delete;
        WorldGenerator& operator=(const WorldGenerator&) = delete;
        WorldGenerator(WorldGenerator&&) = delete;
        WorldGenerator& operator=(WorldGenerator&&) noexcept = delete;

        void GenerateChunk(Chunk& chunk, int chunkX, int chunkY, int chunkZ, float centerX, float centerZ, float radius, uint32_t seed, int totalWorldHeight) const noexcept;
     };
 }