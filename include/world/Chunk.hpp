#pragma once

#include <cstdint>
#include <array>
#include <glm/glm.hpp>

namespace World {
    using VoxelTypeID = uint16_t;

    constexpr int CHUNK_SIZE = 16;
    constexpr int TOTAL_BLOCKS = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

    alignas(64) struct Chunk{
        std::array<VoxelTypeID, TOTAL_BLOCKS> blocks{};

        glm::vec3 worldPosition{0.0f};
        bool isDirty{true};

        [[nodiscard]] constexpr inline VoxelTypeID GetBlock(int x, int y, int z) const noexcept {
          return blocks[static_cast<size_t>(z + (y * CHUNK_SIZE) + (x * CHUNK_SIZE * CHUNK_SIZE))];
    }

        constexpr inline void SetBlock(int x, int y, int z, VoxelTypeID type) noexcept {
            blocks[static_cast<size_t>(z + (y * CHUNK_SIZE) + (x * CHUNK_SIZE * CHUNK_SIZE))] = type;
            isDirty = true;
        }
    };
}
    