#include "../../include/world/Chunk.hpp"
#include "../../include/render/Mesh.hpp"
#include "../../include/world/ChunkBuilder.hpp"

namespace World {
    
void ChunkBuilder::BuildMesh(const Chunk& chunk, Mesh& outMesh) {
    std::vector<Vertex> vertices;
    vertices.reserve(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6); // Rough estimation
            
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            for(int y = 0; y < CHUNK_SIZE; ++y) {
                for(int z = 0; z < CHUNK_SIZE; ++z) {
                    if(chunk.GetBlock(x,y,z) == BlockID::AIR) continue;
                    //Back
                    if ( z == 0 || chunk.GetBlock(x, y, z - 1) == BlockID::AIR)
                        AddBackFace(vertices, x, y, z);
                    //Front
                    if ( z == CHUNK_SIZE - 1 || chunk.GetBlock(x, y, z + 1) == BlockID::AIR)
                        AddFrontFace(vertices, x, y, z);
                    //Left
                     if ( x == 0 || chunk.GetBlock(x - 1, y, z) == BlockID::AIR)
                        AddLeftFace(vertices, x, y, z);
                    //right 
                    if ( x == CHUNK_SIZE - 1 || chunk.GetBlock(x + 1, y, z) == BlockID::AIR)
                        AddRightFace(vertices, x, y, z);
                    //bottom
                    if ( y == 0 || chunk.GetBlock(x, y - 1, z) == BlockID::AIR)
                        AddBottomFace(vertices, x, y, z);
                        //top   
                    if( y == CHUNK_SIZE - 1 || chunk.GetBlock(x, y + 1, z) == BlockID::AIR)
                        AddTopFace(vertices, x, y, z);
                }
            }
        }
        outMesh.Create(vertices);
    }

  void ChunkBuilder::AddTopFace(std::vector<Vertex>& vertices, int x, int y, int z) {
    glm::vec3 pos(x, y, z);
    glm::vec3 norm(0.0f, 1.0f, 0.0f);
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.5f, pos.z + 0.5f}, norm, {0.0f, 0.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f}, norm, {1.0f, 0.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.5f, pos.z - 0.5f}, norm, {1.0f, 1.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.5f, pos.z - 0.5f}, norm, {1.0f, 1.0f}});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.5f, pos.z - 0.5f}, norm, {0.0f, 1.0f}});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.5f, pos.z + 0.5f}, norm, {0.0f, 0.0f}});
}

void ChunkBuilder::AddBottomFace(std::vector<Vertex>& vertices, int x, int y, int z) {
    glm::vec3 pos(x, y, z);
    glm::vec3 norm(0.0f, -1.0f, 0.0f);
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.5f, pos.z - 0.5f}, norm, {0.0f, 0.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.5f, pos.z - 0.5f}, norm, {1.0f, 0.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.5f, pos.z + 0.5f}, norm, {1.0f, 1.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.5f, pos.z + 0.5f}, norm, {1.0f, 1.0f}});
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.5f, pos.z + 0.5f}, norm, {0.0f, 1.0f}});
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.5f, pos.z - 0.5f}, norm, {0.0f, 0.0f}});
}

void ChunkBuilder::AddFrontFace(std::vector<Vertex>& vertices, int x, int y, int z) {
    glm::vec3 pos(x, y, z);
    glm::vec3 norm(0.0f, 0.0f, 1.0f);
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.5f, pos.z + 0.5f}, norm, {0.0f, 0.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.5f, pos.z + 0.5f}, norm, {1.0f, 0.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f}, norm, {1.0f, 1.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f}, norm, {1.0f, 1.0f}});
    vertices.push_back({{-0.5f + pos.x, pos.y + 0.5f, pos.z + 0.5f}, norm, {0.0f, 1.0f}});
    vertices.push_back({{-0.5f + pos.x, pos.y - 0.5f, pos.z + 0.5f}, norm, {0.0f, 0.0f}});
}

void ChunkBuilder::AddBackFace(std::vector<Vertex>& vertices, int x, int y, int z) {
    glm::vec3 pos(x, y, z);
    glm::vec3 norm(0.0f, 0.0f, -1.0f);
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.5f, pos.z - 0.5f}, norm, {0.0f, 0.0f}});
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.5f, pos.z - 0.5f}, norm, {1.0f, 0.0f}});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.5f, pos.z - 0.5f}, norm, {1.0f, 1.0f}});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.5f, pos.z - 0.5f}, norm, {1.0f, 1.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.5f, pos.z - 0.5f}, norm, {0.0f, 1.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.5f, pos.z - 0.5f}, norm, {0.0f, 0.0f}});
}

void ChunkBuilder::AddLeftFace(std::vector<Vertex>& vertices, int x, int y, int z) {
    glm::vec3 pos(x, y, z);
    glm::vec3 norm(-1.0f, 0.0f, 0.0f);
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.5f, pos.z + 0.5f}, norm, {0.0f, 0.0f}});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.5f, pos.z + 0.5f}, norm, {1.0f, 0.0f}});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.5f, pos.z - 0.5f}, norm, {1.0f, 1.0f}});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.5f, pos.z - 0.5f}, norm, {1.0f, 1.0f}});
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.5f, pos.z - 0.5f}, norm, {0.0f, 1.0f}});
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.5f, pos.z + 0.5f}, norm, {0.0f, 0.0f}});
}

void ChunkBuilder::AddRightFace(std::vector<Vertex>& vertices, int x, int y, int z) {
    glm::vec3 pos(x, y, z);
    glm::vec3 norm(1.0f, 0.0f, 0.0f);
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.5f, pos.z - 0.5f}, norm, {0.0f, 0.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.5f, pos.z - 0.5f}, norm, {1.0f, 0.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f}, norm, {1.0f, 1.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f}, norm, {1.0f, 1.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.5f, pos.z + 0.5f}, norm, {0.0f, 1.0f}});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.5f, pos.z - 0.5f}, norm, {0.0f, 0.0f}});
}

}