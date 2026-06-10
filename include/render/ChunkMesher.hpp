#pragma once
#include <vector>
#include "Mesh.hpp"
#include "../world/Chunk.hpp"

namespace Render {

    class ChunkMesher {
    public:


        static void BuildMesh(const World::Chunk& chunk,
                              Mesh& outMesh,
                              const World::Chunk* leftNeighbor   = nullptr,
                              const World::Chunk* rightNeighbor  = nullptr,
                              const World::Chunk* backNeighbor   = nullptr,
                              const World::Chunk* frontNeighbor  = nullptr,
                              const World::Chunk* bottomNeighbor = nullptr,
                              const World::Chunk* topNeighbor    = nullptr
        );
       
    private:

    static bool IsAir(int x, int y, int z, 
                      const World::Chunk& currentChunk,
                      const World::Chunk* left, 
                      const World::Chunk* right, 
                      const World::Chunk* back, 
                      const World::Chunk* front,
                      const World::Chunk* bottom, 
                      const World::Chunk* top ) noexcept;

    static void AddTopFace(std::vector<Vertex>& vertices, int x, int y, int z, World::BlockID id) noexcept;
    static void AddBottomFace(std::vector<Vertex>& vertices, int x, int y, int z, World::BlockID id) noexcept;
    static void AddFrontFace(std::vector<Vertex>& vertices, int x, int y, int z, World::BlockID id) noexcept;
    static void AddBackFace(std::vector<Vertex>& vertices, int x, int y, int z, World::BlockID id) noexcept;
    static void AddLeftFace(std::vector<Vertex>& vertices, int x, int y, int z, World::BlockID id) noexcept;
    static void AddRightFace(std::vector<Vertex>& vertices, int x, int y, int z, World::BlockID id) noexcept;

    };
}
