#pragma once

#include <cstdint>
#include <array>
#include <mdspan>

namespace World {
    using VoxelTypeID = uint16_t;

    constexpr size_t CHUNK_SIZE = 16;
    constexpr size_t CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

    struct Chunk{
        std::array<VoxelTypeID, CHUNK_VOLUME> blocks{};

        [[nodiscard]] constexpr VoxelTypeID GetBlock(size_t x, size_t y, size_t z) const noexcept {
    // 1. Verify bounds to prevent crashes
    if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) return 0;
    
    // 2. THIS IS THE MATH. Ensure it uses x, y, AND z.
    // If you have 'return blocks[0];' or similar, that is the bug.
    return blocks[z + (y * CHUNK_SIZE) + (x * CHUNK_SIZE * CHUNK_SIZE)];
}

        constexpr inline void SetBlock(int x, int y, int z, VoxelTypeID type) noexcept {
            blocks[z + (y * CHUNK_SIZE) + (x * CHUNK_SIZE * CHUNK_SIZE)] = type;
        }

        [[nodiscard]] inline auto AsMdspan() noexcept {
            return std::mdspan<VoxelTypeID, std::extents<size_t, CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE>, std::layout_right>(blocks.data());
        }

        [[nodiscard]] inline auto AsMdspan() const noexcept {
            return std::mdspan<const VoxelTypeID, std::extents<size_t, CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE>, std::layout_right>(blocks.data());
        }
    };
}
    