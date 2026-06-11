#pragma once

#include "Chunk.hpp"
#include "BlockRegistry.hpp"

namespace World {

    struct StrataLayerPalette {
        VoxelTypeID airID;
        VoxelTypeID surfaceID;
        VoxelTypeID subsurfaceID;
        VoxelTypeID baseID;
    };

    class WorldGenerator {
    public:

        WorldGenerator() = default;
        ~WorldGenerator() = default;

        WorldGenerator(const WorldGenerator&) = delete;
        WorldGenerator& operator=(const WorldGenerator&) = delete;

        [[nodiscard]] static StrataLayerPalette CompileDynamicPalette( const BlockRegistryTable& registry) noexcept;
        
        void GenerateChunk(
            Chunk& chunk, 
            int chunkX, 
            int chunkY, 
            int chunkZ, 
            float centerX, 
            float centerZ, 
            float radius, 
            uint32_t seed, 
            int totalWorldHeight,
            const StrataLayerPalette& layers) const noexcept;
     };
 }