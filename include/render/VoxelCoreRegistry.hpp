#pragma once
#include <vector>
#include <array>
#include <cstdint>
#include <glm/glm.hpp>


namespace Render {

    using VoxelTypeID = uint16_t;

    enum class MeshTopology : uint8_t {
        Air         = 0,
        OpaqueCube  = 1,
        Fluid       = 2,
        CrossQuad   = 3    
    };

    struct alignas(4) VoxelSimulationAttributes {
        uint8_t isSolid         :1;
        uint8_t isTransparent   :1;
        uint8_t reserved        : 6;
        uint8_t lightOpacity;
        uint16_t padding;
    };

    struct VoxelCoreRegistry {
        std::vector<VoxelSimulationAttributes> simAttributes;
        std::vector<std::array<int, 6>> textureIndices;
        std::vector<glm::vec4> colors;
        std::vector<MeshTopology> meshTopologies;

        uint32_t textureArrayID{0};

        [[nodiscard]] inline size_t Size() const noexcept {return simAttributes.size();}
    };

    namespace VoxelOps {
        [[nodiscard]] inline bool isSolid(const VoxelCoreRegistry& reg, VoxelTypeID id) noexcept {
            const size_t idx = static_cast<size_t>(id);
            if (idx < reg.simAttributes.size()) [[likely]] {
                return reg.simAttributes[idx].isSolid != 0;
            }
            return false;
        }

        [[nodiscard]] inline bool IsTransparent( const VoxelCoreRegistry& reg, VoxelTypeID id) noexcept {
            const size_t idx = static_cast<size_t>(id);
            if (idx < reg.simAttributes.size()) [[likely]] {
                return reg.simAttributes[idx].isTransparent != 0;
            }
            return true;
        }
        [[nodiscard]] inline MeshTopology GetTopology(const VoxelCoreRegistry& reg, VoxelTypeID id) noexcept {
            const size_t idx = static_cast<size_t>(id);
            if (idx < reg.meshTopologies.size()) [[likely]] {
                return reg.meshTopologies[idx];
            }
            return MeshTopology::Air;
        }
    }
}