#include "../../include/render/ChunkMesher.hpp"
#include "../../include/world/BlockRegistry.hpp"
#include <iostream>

namespace Render {

constexpr float BLOCK_WIDTH = 8.0f;
constexpr float BLOCK_HEIGHT = 1.0f;
constexpr float BLOCK_DEPTH = 8.0f;

constexpr float halfX = BLOCK_WIDTH / 2.0f;
constexpr float halfY = BLOCK_HEIGHT / 2.0f;
constexpr float halfZ = BLOCK_DEPTH / 2.0f;

[[nodiscard]] static inline bool IsTransparent(int x, int y, int z, 
                      auto voxelView,
                      const World::Chunk* left, 
                      const World::Chunk* right, 
                      const World::Chunk* back, 
                      const World::Chunk* front,
                      const World::Chunk* bottom, 
                      const World::Chunk* top,
                      const VoxelCoreRegistry& registry) noexcept
{
    const int maxDim = static_cast<int>(World::CHUNK_SIZE);

    if (x < 0) {
        if(!left) return true;
        return VoxelOps::IsTransparent(registry, left->AsMdspan()[maxDim - 1, y, z]);
    }
    if (x >= maxDim) {
        if(!right) return true;
        return VoxelOps::IsTransparent(registry, right->AsMdspan()[0, y, z]);
    }
    if (y < 0) {
        if(!bottom) return true;
        return VoxelOps::IsTransparent(registry, top->AsMdspan()[x, maxDim - 1, z]);
    }
    if ( y >= maxDim) {
        if (!top) return true;
        return VoxelOps::IsTransparent(registry, top->AsMdspan()[x, 0, z]);
    }
    if (z < 0) {
        if(!back) return true;
        return VoxelOps::IsTransparent(registry, back->AsMdspan()[x, y, maxDim - 1]);    
    }
    if (z >= maxDim) {
        if(!front) return true;
        return VoxelOps::IsTransparent(registry, front->AsMdspan()[x, y, 0]);
    }

    return VoxelOps::IsTransparent(registry, voxelView[x, y, z]);

}

void ChunkMesher::BuildMesh(const World::Chunk& chunk,
                            Mesh& outMesh,
                            const World::Chunk* left,    const World::Chunk* right, 
                            const World::Chunk* back,    const World::Chunk* front,
                            const World::Chunk* bottom, const World::Chunk* top,
                            const VoxelCoreRegistry& registry) noexcept             
{
    auto voxelView = chunk.AsMdspan();

    std::vector<Vertex> vertices;
    vertices.reserve(World::CHUNK_VOLUME);

    for(size_t x = 0; x < World::CHUNK_SIZE; ++x) {
        int ix = static_cast<int>(x);
        for(size_t y = 0; y < World::CHUNK_SIZE; ++y) {
            int iy = static_cast<int>(y);
            for(size_t z = 0; z < World::CHUNK_SIZE; ++z) {
                int iz = static_cast<int>(z);

                const World::VoxelTypeID voxelId = voxelView[x, y, z];
                if (voxelId == 0) continue;

                const MeshTopology topology = VoxelOps::GetTopology(registry, voxelId);
                
                switch (topology) {
                    case MeshTopology::OpaqueCube: {

                        if (iy == static_cast<int>(World::CHUNK_SIZE) - 1) {
                            if (IsTransparent(ix, iy + 1, iz, voxelView, left, right, back, front, bottom, top, registry)) 
                                AddTopFace(vertices, ix, iy, iz, voxelId, registry);
                        } else if (VoxelOps::IsTransparent(registry, voxelView[x, y + 1, z])) {
                            AddTopFace(vertices, ix, iy, iz, voxelId, registry);
                        }

                        if (iy == 0) {
                            if (IsTransparent(ix, iy - 1, iz, voxelView, left, right, back, front, bottom, top, registry))
                                AddBottomFace(vertices, ix, iy, iz, voxelId, registry);
                        } else if (VoxelOps::IsTransparent(registry, voxelView[x, y - 1, z])) {
                            AddBottomFace(vertices, ix, iy, iz, voxelId, registry);
                        }

                        if (IsTransparent(ix, iy, iz + 1, voxelView, left, right, back, front, bottom, top, registry)) AddFrontFace(vertices, ix, iy, iz, voxelId, registry);
                        if (IsTransparent(ix, iy, iz - 1, voxelView, left, right, back, front, bottom, top, registry)) AddBackFace(vertices, ix, iy, iz, voxelId, registry);
                        if (IsTransparent(ix - 1, iy, iz, voxelView, left, right, back, front, bottom, top, registry)) AddLeftFace(vertices, ix, iy, iz, voxelId, registry);
                        if (IsTransparent(ix + 1, iy, iz, voxelView, left, right, back, front, bottom, top, registry)) AddRightFace(vertices, ix, iy, iz, voxelId, registry);    
                        break;
                    }

                    case MeshTopology::CrossQuad: {
                        AddCrossQuadGeometry(vertices, ix, iy, iz, voxelId, registry);
                        break;
                    }
                    case MeshTopology::Fluid:
                    case MeshTopology::Air:
                    default:
                        break;
                    }
                }
            }
        }
        outMesh.Create(vertices);
    }

void ChunkMesher::AddTopFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * BLOCK_WIDTH, static_cast<float>(y) * BLOCK_HEIGHT, static_cast<float>(z) * BLOCK_DEPTH);
    glm::vec3 norm(0.0f, 1.0f, 0.0f);
    glm::vec4 col = registry.colors[id];
    col.a = static_cast<float>(registry.textureIndices[id][0]);
    
    // Top face stays a perfect square (8x8), standard 0.0 to 1.0 UV mapping works perfectly
    vertices.push_back({{pos.x - halfX, pos.y + halfY, pos.z - halfZ}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x - halfX, pos.y + halfY, pos.z + halfZ}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z + halfZ}, norm, {1.0f, 0.0f}, col});
    
    vertices.push_back({{pos.x - halfX, pos.y + halfY, pos.z - halfZ}, norm, {0.0f, 1.0f}, col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z + halfZ}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z - halfZ}, norm, {1.0f, 1.0f}, col});
}

void ChunkMesher::AddBottomFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * BLOCK_WIDTH, static_cast<float>(y) * BLOCK_HEIGHT, static_cast<float>(z) * BLOCK_DEPTH);
    glm::vec3 norm(0.0f, -1.0f, 0.0f);
    glm::vec4 col = registry.colors[id];
    col.a = static_cast<float>(registry.textureIndices[id][1]);

    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z - halfZ}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + halfX, pos.y - halfY, pos.z - halfZ}, norm, {1.0f, 0.0f}, col});
    vertices.push_back({{pos.x + halfX, pos.y - halfY, pos.z + halfZ}, norm, {1.0f, 1.0f}, col});
    
    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z - halfZ}, norm, {0.0f, 0.0f}, col});
    vertices.push_back({{pos.x + halfX, pos.y - halfY, pos.z + halfZ}, norm, {1.0f, 1.0f}, col});
    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z + halfZ}, norm, {0.0f, 1.0f}, col});
}

void ChunkMesher::AddFrontFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * BLOCK_WIDTH, static_cast<float>(y) * BLOCK_HEIGHT, static_cast<float>(z) * BLOCK_DEPTH);
    glm::vec3 norm(0.0f, 0.0f, 1.0f);
    
    glm::vec4 col = registry.colors[id];
    col.a = static_cast<float>(registry.textureIndices[id][2]);

    float vBottom = static_cast<float>(y) / 8.0f;
    float vTop    = static_cast<float>(y + 1) / 8.0f;

    // 🔑 FIX: Replaced old hardcoded vertical UVs with vBottom and vTop variables
    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z + halfZ}, norm, {0.0f, vBottom}, col});
    vertices.push_back({{pos.x + halfX, pos.y - halfY, pos.z + halfZ}, norm, {1.0f, vBottom}, col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z + halfZ}, norm, {1.0f, vTop},    col});
    
    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z + halfZ}, norm, {0.0f, vBottom}, col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z + halfZ}, norm, {1.0f, vTop},    col});
    vertices.push_back({{pos.x - halfX, pos.y + halfY, pos.z + halfZ}, norm, {0.0f, vTop},    col});
}

void ChunkMesher::AddBackFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * BLOCK_WIDTH, static_cast<float>(y) * BLOCK_HEIGHT, static_cast<float>(z) * BLOCK_DEPTH);
    glm::vec3 norm(0.0f, 0.0f, -1.0f);
    
    glm::vec4 col = registry.colors[id];
    col.a = static_cast<float>(registry.textureIndices[id][3]);

    float vBottom = static_cast<float>(y) / 8.0f;
    float vTop    = static_cast<float>(y + 1) / 8.0f;

    // 🔑 FIX: Replaced old hardcoded vertical UVs with vBottom and vTop variables
    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z - halfZ}, norm, {1.0f, vBottom}, col});
    vertices.push_back({{pos.x - halfX, pos.y + halfY, pos.z - halfZ}, norm, {1.0f, vTop},    col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z - halfZ}, norm, {0.0f, vTop},    col});
    
    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z - halfZ}, norm, {1.0f, vBottom}, col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z - halfZ}, norm, {0.0f, vTop},    col});
    vertices.push_back({{pos.x + halfX, pos.y - halfY, pos.z - halfZ}, norm, {0.0f, vBottom}, col});
}

void ChunkMesher::AddLeftFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * BLOCK_WIDTH, static_cast<float>(y) * BLOCK_HEIGHT, static_cast<float>(z) * BLOCK_DEPTH);
    glm::vec3 norm(-1.0f, 0.0f, 0.0f);
    
    glm::vec4 col = registry.colors[id];
    col.a = static_cast<float>(registry.textureIndices[id][4]);

    float vBottom = static_cast<float>(y) / 8.0f;
    float vTop    = static_cast<float>(y + 1) / 8.0f;

    // 🔑 FIX: Replaced old hardcoded vertical UVs with vBottom and vTop variables
    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z - halfZ}, norm, {0.0f, vBottom}, col});
    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z + halfZ}, norm, {1.0f, vBottom}, col});
    vertices.push_back({{pos.x - halfX, pos.y + halfY, pos.z + halfZ}, norm, {1.0f, vTop},    col});
    
    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z - halfZ}, norm, {0.0f, vBottom}, col});
    vertices.push_back({{pos.x - halfX, pos.y + halfY, pos.z + halfZ}, norm, {1.0f, vTop},    col});
    vertices.push_back({{pos.x - halfX, pos.y + halfY, pos.z - halfZ}, norm, {0.0f, vTop},    col});
}

void ChunkMesher::AddRightFace(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept {
    glm::vec3 pos(static_cast<float>(x) * BLOCK_WIDTH, static_cast<float>(y) * BLOCK_HEIGHT, static_cast<float>(z) * BLOCK_DEPTH);
    glm::vec3 norm(1.0f, 0.0f, 0.0f);
    
    glm::vec4 col = registry.colors[id];
    col.a = static_cast<float>(registry.textureIndices[id][5]);

    float vBottom = static_cast<float>(y) / 8.0f;
    float vTop    = static_cast<float>(y + 1) / 8.0f;

    // 🔑 FIX: Replaced old hardcoded vertical UVs with vBottom and vTop variables
    vertices.push_back({{pos.x + halfX, pos.y - halfY, pos.z - halfZ}, norm, {1.0f, vBottom}, col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z - halfZ}, norm, {1.0f, vTop},    col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z + halfZ}, norm, {0.0f, vTop},    col});
    
    vertices.push_back({{pos.x + halfX, pos.y - halfY, pos.z - halfZ}, norm, {1.0f, vBottom}, col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z + halfZ}, norm, {0.0f, vTop},    col});
    vertices.push_back({{pos.x + halfX, pos.y - halfY, pos.z + halfZ}, norm, {0.0f, vBottom}, col});
}

void ChunkMesher::AddCrossQuadGeometry(std::vector<Vertex>& vertices, int x, int y, int z, World::VoxelTypeID id, const VoxelCoreRegistry& registry) noexcept {
    const glm::vec3 pos(static_cast<float>(x) * BLOCK_WIDTH, static_cast<float>(y) * BLOCK_HEIGHT, static_cast<float>(z) * BLOCK_DEPTH);
    constexpr glm::vec3 norm(0.0f, 1.0f, 0.0f);
    glm::vec4 col = registry.colors[id];
    col.a = static_cast<float>(registry.textureIndices[id][0]);

    const float vBottom = static_cast<float>(y) / 8.0f;
    const float vTop    = static_cast<float>(y + 1) / 8.0f;

    // --- Quad Plane 1: Diagonal from Back-Left to Front-Right ---
    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z - halfZ}, norm, {0.0f, vBottom}, col});
    vertices.push_back({{pos.x + halfX, pos.y - halfY, pos.z + halfZ}, norm, {1.0f, vBottom}, col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z + halfZ}, norm, {1.0f, vTop},    col});
    
    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z - halfZ}, norm, {0.0f, vBottom}, col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z + halfZ}, norm, {1.0f, vTop},    col});
    vertices.push_back({{pos.x - halfX, pos.y + halfY, pos.z - halfZ}, norm, {0.0f, vTop},    col});

    // --- Quad Plane 2: Diagonal from Front-Left to Back-Right ---
    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z + halfZ}, norm, {0.0f, vBottom}, col});
    vertices.push_back({{pos.x + halfX, pos.y - halfY, pos.z - halfZ}, norm, {1.0f, vBottom}, col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z - halfZ}, norm, {1.0f, vTop},    col});
    
    vertices.push_back({{pos.x - halfX, pos.y - halfY, pos.z + halfZ}, norm, {0.0f, vBottom}, col});
    vertices.push_back({{pos.x + halfX, pos.y + halfY, pos.z - halfZ}, norm, {1.0f, vTop},    col});
    vertices.push_back({{pos.x - halfX, pos.y + halfY, pos.z + halfZ}, norm, {0.0f, vTop},    col});
}

} // namespace Render