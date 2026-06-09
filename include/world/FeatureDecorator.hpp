#pragma once

#include <vector>
#include "Chunk.hpp"

namespace World {

    class FeatureDecorator {
    public:
        FeatureDecorator() = default;
        ~FeatureDecorator() = default;

        void DecorateWorld(std::vector<Chunk>& chunks, uint32_t seed) const noexcept;
        
    private:

    [[nodiscard]] bool IsAreaFlat(const Chunk& chunk, int startX, int StartY, int StartZ, int width, int depth) const noexcept; 
        
    void SpawnRockOutcrop(Chunk& chunk, int tx, int ty, int tz) const noexcept;
    };
}