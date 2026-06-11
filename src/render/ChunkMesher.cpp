#include "../../include/render/ChunkMesher.hpp"

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

    if (y < 0)  return bottom ? (bottom->GetBlock(static_cast<size_t>(x), World::CHUNK_SIZE - 1, static_cast<size_t>(z)) == 0) : true;
    if (y >= static_cast<int>(World::CHUNK_SIZE)) return top    ? (top->GetBlock(static_cast<size_t>(x), 0, static_cast<size_t>(z)) == 0) : true;
    if (x < 0)  return left   ? (left->GetBlock(World::CHUNK_SIZE - 1, static_cast<size_t>(y), static_cast<size_t>(z)) == 0) : true; 
    if (x >= static_cast<int>(World::CHUNK_SIZE)) return right  ? (right->GetBlock(0, static_cast<size_t>(y), static_cast<size_t>(z)) == 0) : true;
    if (z < 0)  return back   ? (back->GetBlock(static_cast<size_t>(x), static_cast<size_t>(y), World::CHUNK_SIZE - 1) == 0) : true;
    if (z >= static_cast<int>(World::CHUNK_SIZE)) return front  ? (front->GetBlock(static_cast<size_t>(x), static_cast<size_t>(y), 0) == 0) : true;

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

    for(int x = 0; x < World::CHUNK_SIZE; ++x) {
        for(int y = 0; y < World::CHUNK_SIZE; ++y) {
            for(int z = 0; z < World::CHUNK_SIZE; ++z) {

                World::VoxelTypeID block = chunk.GetBlock(x, y, z);
                if (block == 0) continue;
                
                if ( y == World::CHUNK_SIZE - 1) {
                    if (IsAir(x, y + 1, z, chunk, left, right, back, front, bottom, top)) AddTopFace(vertices, x, y, z, block, registry);
                } else if (chunk.GetBlock(x, y + 1, z) ==0) {
                    AddTopFace(vertices, x, y, z, block, registry);
                }
                if (y == 0){
                    if (IsAir(x, y -1, z, chunk, left, right, back, front, bottom, top)) AddBottomFace(vertices, x, y, z, block, registry);
                } else if (chunk.GetBlock(x, y -1, z) == 0) {
                    AddBottomFace(vertices, x, y, z, block, registry);
                }

                if (IsAir(x, y, z + 1, chunk, left, right, back, front, bottom, top)){ AddFrontFace(vertices, x, y, z, block, registry);}
                if (IsAir(x, y, z - 1, chunk, left, right, back, front, bottom, top)){ AddBackFace(vertices, x, y, z, block, registry);}
                if (IsAir(x - 1, y, z, chunk, left, right, back, front, bottom, top)){ AddLeftFace(vertices, x, y, z, block, registry);}
                if (IsAir(x + 1, y, z, chunk, left, right, back, front, bottom, top)){ AddRightFace(vertices, x, y, z, block, registry);}    
            }
        }
    }
    outMesh.Create(vertices);
}

// === Half-Height Slabs Geometry Layout Generator (1.0 x 0.5 x 1.0) ===

void ChunkMesher::AddTopFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const World::BlockRegistryTable& registry) noexcept {
    // Spatial placement stepped dynamically to reflect macro spacing footprints
    glm::vec3 pos(static_cast<float>(x) * 2.0f, static_cast<float>(y) * 1.0f, static_cast<float>(z) * 2.0f);
    glm::vec3 norm(0.0f, 1.0f, 0.0f);
    glm::vec4 col = World::BlockOps::GetColor(registry, id);
    
    // Extrusion offsets set to Half-Extents (X/Z = 1.0f, Y = 0.5f)
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {0.0f, 0.0f}, col});
}

void ChunkMesher::AddBottomFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const World::BlockRegistryTable& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * 2.0f, static_cast<float>(y) * 1.0f, static_cast<float>(z) * 2.0f);
    glm::vec3 norm(0.0f, -1.0f, 0.0f);
    glm::vec4 col = World::BlockOps::GetColor(registry, id);

    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {0.0f, 0.0f}, col});
}

void ChunkMesher::AddFrontFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const World::BlockRegistryTable& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * 2.0f, static_cast<float>(y) * 1.0f, static_cast<float>(z) * 2.0f);
    glm::vec3 norm(0.0f, 0.0f, 1.0f);
    glm::vec4 col = World::BlockOps::GetColor(registry, id);

    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {0.0f, 0.0f}, col});
}

void ChunkMesher::AddBackFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const World::BlockRegistryTable& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * 2.0f, static_cast<float>(y) * 1.0f, static_cast<float>(z) * 2.0f);
    glm::vec3 norm(0.0f, 0.0f, -1.0f);
    glm::vec4 col = World::BlockOps::GetColor(registry, id);

    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {0.0f, 0.0f}, col});
}

void ChunkMesher::AddLeftFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const World::BlockRegistryTable& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * 2.0f, static_cast<float>(y) * 1.0f, static_cast<float>(z) * 2.0f);
    glm::vec3 norm(1.0f, 0.0f, 0.0f);
    glm::vec4 col = World::BlockOps::GetColor(registry, id);

    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {0.0f, 0.0f}, col});
}

void ChunkMesher::AddRightFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const World::BlockRegistryTable& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * 2.0f, static_cast<float>(y) * 1.0f, static_cast<float>(z) * 2.0f);
    glm::vec3 norm(1.0f, 0.0f, 0.0f);
    glm::vec4 col = World::BlockOps::GetColor(registry, id);

    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z - 1.0f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y + 0.5f, pos.z + 1.0f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z + 1.0f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 1.0f, pos.y - 0.5f, pos.z - 1.0f}, norm, {0.0f, 0.0f}, col});
}

} // namespace Render