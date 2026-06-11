#include "../../include/render/ChunkMesher.hpp"
#include "../../include/world/BlockRegistry.hpp"
#include <iostream>

namespace Render {

bool ChunkMesher::IsAir(int x, int y, int z, 
                      const World::Chunk& currentChunk,
                      const World::Chunk* left, 
                      const World::Chunk* right, 
                      const World::Chunk* back, 
                      const World::Chunk* front,
                      const World::Chunk* bottom, 
                      const World::Chunk* top ) noexcept
{
    const int maxDim = static_cast<int>(World::CHUNK_SIZE);

    if (x < 0)       return left ? (left->GetBlock(maxDim - 1, y, z) == 0) : true; 
    if (x >= maxDim) return right ? (right->GetBlock(0, y, z) == 0) : true;
    if (y < 0)       return bottom ? (bottom->GetBlock(x, maxDim - 1, z) == 0) : true;
    if (y >= maxDim) return top ? (top->GetBlock(x, 0, z) == 0) : true;
    if (z < 0)       return back ? (back->GetBlock(x, y, maxDim - 1) == 0) : true;
    if (z >= maxDim) return front ? (front->GetBlock(x, y, 0) == 0) : true;

    return currentChunk.GetBlock(static_cast<size_t>(x), static_cast<size_t>(y), static_cast<size_t>(z)) == 0;
}

void ChunkMesher::BuildMesh(const World::Chunk& chunk,
                            Mesh& outMesh,
                            const World::Chunk* left,    const World::Chunk* right, 
                            const World::Chunk* back,    const World::Chunk* front,
                            const World::Chunk* bottom, const World::Chunk* top,
                            const World::BlockRegistryTable& registry) noexcept             
{
    std::vector<Vertex> vertices;
    vertices.reserve(World::CHUNK_VOLUME * 2);

    for(size_t x = 0; x < World::CHUNK_SIZE; ++x) {
        int ix = static_cast<int>(x);
        for(size_t y = 0; y < World::CHUNK_SIZE; ++y) {
            int iy = static_cast<int>(y);
            for(size_t z = 0; z < World::CHUNK_SIZE; ++z) {
                int iz = static_cast<int>(z);

                World::VoxelTypeID block = chunk.GetBlock(x, y, z);
                if (block == 0) continue;
                
                if (y == World::CHUNK_SIZE - 1ULL) {
                    if (IsAir(ix, iy + 1, iz, chunk, left, right, back, front, bottom, top)) 
                        AddTopFace(vertices, ix, iy, iz, block, registry);
                } else if (chunk.GetBlock(x, y + 1ULL, z) == 0) {
                    AddTopFace(vertices, ix, iy, iz, block, registry);
                }

                if (y == 0ULL) {
                    if (IsAir(ix, iy - 1, iz, chunk, left, right, back, front, bottom, top)) 
                        AddBottomFace(vertices, ix, iy, iz, block, registry);
                } else if (chunk.GetBlock(x, y - 1ULL, z) == 0) {
                    AddBottomFace(vertices, ix, iy, iz, block, registry);
                }

                if (IsAir(ix, iy, iz + 1, chunk, left, right, back, front, bottom, top)) AddFrontFace(vertices, ix, iy, iz, block, registry);
                if (IsAir(ix, iy, iz - 1, chunk, left, right, back, front, bottom, top)) AddBackFace(vertices, ix, iy, iz, block, registry);
                if (IsAir(ix - 1, iy, iz, chunk, left, right, back, front, bottom, top)) AddLeftFace(vertices, ix, iy, iz, block, registry);
                if (IsAir(ix + 1, iy, iz, chunk, left, right, back, front, bottom, top)) AddRightFace(vertices, ix, iy, iz, block, registry);    
            }
        }
    }
    outMesh.Create(vertices);
}

void ChunkMesher::AddTopFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const World::BlockRegistryTable& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * 2.0f, static_cast<float>(y) * 1.0f, static_cast<float>(z) * 2.0f);
    glm::vec3 norm(0.0f, 1.0f, 0.0f);
    glm::vec4 col = registry.colors[id];
    
    // Horizontal plane at Y = +0.5
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {1.0f, 0.0f}, col});
    
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {1.0f, 1.0f}, col});
}

void ChunkMesher::AddBottomFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const World::BlockRegistryTable& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * 2.0f, static_cast<float>(y) * 1.0f, static_cast<float>(z) * 2.0f);
    glm::vec3 norm(0.0f, -1.0f, 0.0f);
    glm::vec4 col = registry.colors[id];

    // Horizontal plane at Y = -0.5
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {1.0f, 1.0f}, col});
    
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {0.0f, 1.0f}, col});
}

void ChunkMesher::AddFrontFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const World::BlockRegistryTable& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * 2.0f, static_cast<float>(y) * 1.0f, static_cast<float>(z) * 2.0f);
    glm::vec3 norm(0.0f, 0.0f, 1.0f);
    glm::vec4 col = registry.colors[id];

    // Vertical wall at Z = +1.0
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {1.0f, 1.0f}, col});
    
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {0.0f, 1.0f}, col});
}

void ChunkMesher::AddBackFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const World::BlockRegistryTable& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * 2.0f, static_cast<float>(y) * 1.0f, static_cast<float>(z) * 2.0f);
    glm::vec3 norm(0.0f, 0.0f, -1.0f);
    glm::vec4 col = registry.colors[id];

    // Vertical wall at Z = -1.0
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {0.0f, 1.0f}, col});
    
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {0.0f, 0.0f}, col});
}

void ChunkMesher::AddLeftFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const World::BlockRegistryTable& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * 2.0f, static_cast<float>(y) * 1.0f, static_cast<float>(z) * 2.0f);
    glm::vec3 norm(-1.0f, 0.0f, 0.0f);
    glm::vec4 col = registry.colors[id];

    // Vertical wall at X = -1.0
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {1.0f, 1.0f}, col});
    
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {0.0f, 1.0f}, col});
}

void ChunkMesher::AddRightFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const World::BlockRegistryTable& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * 2.0f, static_cast<float>(y) * 1.0f, static_cast<float>(z) * 2.0f);
    glm::vec3 norm(1.0f, 0.0f, 0.0f);
    glm::vec4 col = registry.colors[id];

    // Vertical wall at X = +1.0
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {0.0f, 1.0f}, col});
    
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {0.0f, 0.0f}, col});
}

} // namespace Render