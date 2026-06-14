#pragma once
#include <vector>
#include "Mesh.hpp"
#include "../world/Chunk.hpp"
#include "VoxelCoreRegistry.hpp"


namespace Render {

    class ChunkMesher {
    public:


        static void BuildMesh(const World::Chunk& chunk,
                              Mesh& outMesh,
                              const World::Chunk* leftNeighbor,
                              const World::Chunk* rightNeighbor,
                              const World::Chunk* backNeighbor,
                              const World::Chunk* frontNeighbor,
                              const World::Chunk* bottomNeighbor,
                              const World::Chunk* topNeighbor,
                              const VoxelCoreRegistry& registry) noexcept;
    
    private:

    static bool IsAir(int x, int y, int z, 
                      const World::Chunk& currentChunk,
                      const World::Chunk* left, 
                      const World::Chunk* right, 
                      const World::Chunk* back, 
                      const World::Chunk* front,
                      const World::Chunk* bottom, 
                      const World::Chunk* top ) noexcept;

    static void AddTopFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept;
    static void AddBottomFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept;
    static void AddFrontFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept;
    static void AddBackFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept;
    static void AddLeftFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept;
    static void AddRightFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept;
    static void AddCrossQuadGeometry(std::vector<Vertex>& vetices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept;
    };
}