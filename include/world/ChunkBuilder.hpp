# pragma once

#include "Chunk.hpp"
#include "../render/Mesh.hpp"
#include <vector>

namespace World {
    class ChunkBuilder {
    public:
        static void BuildMesh(const Chunk& chunk, Mesh& outMesh);

    private:

        static void AddTopFace(std::vector<Vertex>& vertices, int x, int y, int z);
        static void AddBottomFace(std::vector<Vertex>& vertices, int x, int y, int z);
        static void AddFrontFace(std::vector<Vertex>& vertices, int x, int y, int z);
        static void AddBackFace(std::vector<Vertex>& vertices, int x, int y, int z);
        static void AddLeftFace(std::vector<Vertex>& vertices, int x, int y, int z);
        static void AddRightFace(std::vector<Vertex>& vertices, int x, int y, int z);
    };
}
