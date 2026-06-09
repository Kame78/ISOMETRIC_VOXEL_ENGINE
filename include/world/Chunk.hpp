#pragma once

#include <cstdint>
#include <array>
#include <glm/glm.hpp>

namespace World {
    constexpr int CHUNK_SIZE = 16;
    constexpr int TOTAL_BLOCKS = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

    enum class BlockID : uint8_t {
        AIR = 0,
        DIRT, 
        STONE,
        GRASS
    };

    struct Chunk {
        std::array<BlockID, TOTAL_BLOCKS> blocks{ BlockID::AIR };
        glm::vec3 worldPosition{0.0f};
        bool isDirty{ true };
    
       [[nodiscard]] constexpr inline BlockID GetBlock(int x, int y, int z) const noexcept {
            return blocks[static_cast<size_t>(x + (y * CHUNK_SIZE) + (z * CHUNK_SIZE * CHUNK_SIZE))];
        }

        constexpr inline void SetBlock(int x, int y, int z, BlockID type) noexcept {
            blocks[static_cast<size_t>(x + (y * CHUNK_SIZE) + (z * CHUNK_SIZE * CHUNK_SIZE))] = type;
            isDirty = true;
        }
    };
}