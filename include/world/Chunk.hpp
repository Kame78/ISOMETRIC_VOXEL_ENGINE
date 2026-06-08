#pragma once

#include <cstdint>
#include <array>
#include <glm/glm.hpp>

namespace World {
    constexpr int CHUNK_SIZE = 16;

    enum class BlockID : uint8_t {
        AIR = 0,
        DIRT, 
        STONE,
        GRASS
    };

    struct Chunk {
        std::array<BlockID, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> blocks;
        glm::vec3 worldPosition{0.0f};
        bool isDirty{ true };
    
        BlockID GetBlock(int x, int y, int z) const {
            return blocks[x +(y * CHUNK_SIZE) + (z * CHUNK_SIZE * CHUNK_SIZE)];
        }

        void SetBlock(int x, int y, int z, BlockID type) {
            blocks[x + (y * CHUNK_SIZE) + (z * CHUNK_SIZE * CHUNK_SIZE)] = type;
            isDirty = true;
        }
    };
}