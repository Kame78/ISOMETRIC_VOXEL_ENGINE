#pragma once

#include "Chunk.hpp"
#include "BlockRegistry.hpp"

namespace World {

    struct StrataLayerPalette {
        VoxelTypeID airID;
        VoxelTypeID surfaceID;
        VoxelTypeID subsurfaceID;
        VoxelTypeID baseID;
        VoxelTypeID waterID;
    };

    class WorldGenerator {
    public:

        WorldGenerator() = default;
        ~WorldGenerator() = default;
        
        void GenerateChunk(
            Chunk& chunk, 
            int cx, 
            int cy, 
            int cz, 
            float centerX, 
            float centerZ, 
            float radius, 
            uint32_t seed, 
            int totalWorldHeight,
            const StrataLayerPalette& palette) const noexcept;
     };
 }