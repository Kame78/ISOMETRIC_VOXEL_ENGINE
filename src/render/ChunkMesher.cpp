#include "../../include/render/ChunkMesher.hpp"
#include "../../include/world/BlockRegistry.hpp"

namespace Render {

bool ChunkMesher::IsAir(int x, int y, int z, 
                      const World::Chunk& currentChunk,
                      const World::Chunk* left, 
                      const World::Chunk* right, 
                      const World::Chunk* back, 
                      const World::Chunk* front) noexcept
{

    if (y < 0 || y>= World::CHUNK_SIZE) return true;

    if (x < 0) {
        return left ? (left->GetBlock(World::CHUNK_SIZE - 1, y, z) == World::BlockID::AIR) : true; 
    }

    if ( x >= World::CHUNK_SIZE) {
        return right ? (right->GetBlock(0, y, z) == World::BlockID::AIR) : true;
    }

    if (z < 0) {
        return back ? (back->GetBlock(x, y, World::CHUNK_SIZE - 1) == World::BlockID::AIR) : true;
    }

    if (z >= World::CHUNK_SIZE) {
        return front ? (front->GetBlock(x, y, 0) == World::BlockID::AIR) : true;
    }

    return currentChunk.GetBlock(x, y, z) == World::BlockID::AIR;
}

void ChunkMesher::BuildMesh(const World::Chunk& chunk,
                            Mesh& outMesh,
                            const World::Chunk* left,    const World::Chunk* right, 
                            const World::Chunk* back,    const World::Chunk* front)
{
    std::vector<Vertex> vertices;
    vertices.reserve(World::CHUNK_SIZE * World::CHUNK_SIZE * World::CHUNK_SIZE * 2);

    for(int x = 0; x < World::CHUNK_SIZE; ++x) {
        for(int y = 0; y < World::CHUNK_SIZE; ++y) {
            for(int z = 0; z < World::CHUNK_SIZE; ++z) {

                World::BlockID block = chunk.GetBlock(x, y, z);
                if (block == World::BlockID::AIR) continue;
                
                if ( y == World::CHUNK_SIZE - 1 || chunk.GetBlock(x, y + 1, z) == World::BlockID::AIR) {
                    AddTopFace(vertices, x, y, z, block);
                }

                if( y == 0 || chunk.GetBlock(x, y - 1, z) == World::BlockID::AIR) {
                    AddBottomFace(vertices, x, y, z, block);
                }

                if (IsAir(x, y, z + 1, chunk, left, right, back, front)){
                    AddFrontFace(vertices, x, y, z, block);
                }
                if (IsAir(x, y, z - 1, chunk, left, right, back, front)){
                    AddBackFace(vertices, x, y, z, block);
                }
                if (IsAir(x - 1, y, z, chunk, left, right, back, front)){
                    AddLeftFace(vertices, x, y, z, block);
                }
                if (IsAir(x + 1, y, z, chunk, left, right, back, front)){
                    AddRightFace(vertices, x, y, z, block);
                }    
            }
        }
    }
    outMesh.Create(vertices);
}

// === Half-Height Slabs Geometry Layout Generator (1.0 x 0.5 x 1.0) ===

void ChunkMesher::AddTopFace(std::vector<Vertex>& vertices, int x, int y, int z, World::BlockID id) noexcept {
    glm::vec3 pos(static_cast<float>(x), static_cast<float>(y) * 0.5f, static_cast<float>(z));
    glm::vec3 norm(0.0f, 1.0f, 0.0f);

    const auto& props = World::BlockRegistry::GetProperties(id);
    glm::vec4 col = props.color;
    
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.25f, pos.z + 0.5f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.25f, pos.z + 0.5f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.25f, pos.z - 0.5f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.25f, pos.z - 0.5f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.25f, pos.z - 0.5f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.25f, pos.z + 0.5f}, norm, {0.0f, 0.0f}, col});
}

void ChunkMesher::AddBottomFace(std::vector<Vertex>& vertices, int x, int y, int z, World::BlockID id) noexcept {
    glm::vec3 pos(static_cast<float>(x), static_cast<float>(y) * 0.5f, static_cast<float>(z));
    glm::vec3 norm(0.0f, -1.0f, 0.0f);
    
    const auto& props = World::BlockRegistry::GetProperties(id);
    glm::vec4 col = props.color;

    vertices.push_back({{pos.x - 0.5f, pos.y - 0.25f, pos.z - 0.5f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.25f, pos.z - 0.5f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.25f, pos.z + 0.5f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.25f, pos.z + 0.5f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.25f, pos.z + 0.5f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.25f, pos.z - 0.5f}, norm, {0.0f, 0.0f}, col});
}

void ChunkMesher::AddFrontFace(std::vector<Vertex>& vertices, int x, int y, int z, World::BlockID id) noexcept {
    glm::vec3 pos(static_cast<float>(x), static_cast<float>(y) * 0.5f, static_cast<float>(z));
    glm::vec3 norm(0.0f, 0.0f, 1.0f);
    
    const auto& props = World::BlockRegistry::GetProperties(id);
    glm::vec4 col = props.color;

    vertices.push_back({{pos.x - 0.5f, pos.y - 0.25f, pos.z + 0.5f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.25f, pos.z + 0.5f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.25f, pos.z + 0.5f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.25f, pos.z + 0.5f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.25f, pos.z + 0.5f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.25f, pos.z + 0.5f}, norm, {0.0f, 0.0f}, col});
}

void ChunkMesher::AddBackFace(std::vector<Vertex>& vertices, int x, int y, int z, World::BlockID id) noexcept {
    glm::vec3 pos(static_cast<float>(x), static_cast<float>(y) * 0.5f, static_cast<float>(z));
    glm::vec3 norm(0.0f, 0.0f, -1.0f);
    
    const auto& props = World::BlockRegistry::GetProperties(id);
    glm::vec4 col = props.color;

    vertices.push_back({{pos.x + 0.5f, pos.y - 0.25f, pos.z - 0.5f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.25f, pos.z - 0.5f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.25f, pos.z - 0.5f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.25f, pos.z - 0.5f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.25f, pos.z - 0.5f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.25f, pos.z - 0.5f}, norm, {0.0f, 0.0f}, col});
}

void ChunkMesher::AddLeftFace(std::vector<Vertex>& vertices, int x, int y, int z, World::BlockID id) noexcept {
    glm::vec3 pos(static_cast<float>(x), static_cast<float>(y) * 0.5f, static_cast<float>(z));
    glm::vec3 norm(-1.0f, 0.0f, 0.0f);
    
    const auto& props = World::BlockRegistry::GetProperties(id);
    glm::vec4 col = props.color;

    vertices.push_back({{pos.x - 0.5f, pos.y - 0.25f, pos.z + 0.5f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.25f, pos.z + 0.5f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.25f, pos.z - 0.5f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y + 0.25f, pos.z - 0.5f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.25f, pos.z - 0.5f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x - 0.5f, pos.y - 0.25f, pos.z + 0.5f}, norm, {0.0f, 0.0f}, col});
}

void ChunkMesher::AddRightFace(std::vector<Vertex>& vertices, int x, int y, int z, World::BlockID id) noexcept {
    glm::vec3 pos(static_cast<float>(x), static_cast<float>(y) * 0.5f, static_cast<float>(z));
    glm::vec3 norm(1.0f, 0.0f, 0.0f);
   
    const auto& props = World::BlockRegistry::GetProperties(id);
    glm::vec4 col = props.color;

    vertices.push_back({{pos.x + 0.5f, pos.y - 0.25f, pos.z - 0.5f}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.25f, pos.z - 0.5f}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.25f, pos.z + 0.5f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y + 0.25f, pos.z + 0.5f}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.25f, pos.z + 0.5f}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x + 0.5f, pos.y - 0.25f, pos.z - 0.5f}, norm, {0.0f, 0.0f}, col});
}

} // namespace Render