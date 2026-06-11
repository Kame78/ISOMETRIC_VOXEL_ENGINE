#pragma once

#include <cstdint>
#include <array>
#include <mdspan>

namespace World {
    using VoxelTypeID = uint16_t;

    constexpr int CHUNK_SIZE = 16;
    constexpr int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

    struct Chunk{
        std::array<VoxelTypeID, CHUNK_VOLUME> blocks{};

        [[nodiscard]] constexpr inline VoxelTypeID GetBlock(int x, int y, int z) const noexcept {
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
    