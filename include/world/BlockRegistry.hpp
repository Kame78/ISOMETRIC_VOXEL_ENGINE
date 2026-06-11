#pragma once

#include <string>
#include <vector>
#include <array>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <glm/glm.hpp>

namespace World {

    using VoxelTypeID = uint16_t;

    struct TransparentStringHash{
        using is_transparent = void;

        [[nodiscard]] size_t operator()(std::string_view sv) const noexcept {
            return std::hash<std::string_view>{}(sv);
        }

        [[nodiscard]] size_t operator()(const std::string& s) const noexcept {
            return std::hash<std::string>{}(s);
        }
    };

    struct alignas(4) SimulationAttributes {
        uint8_t isSolid         :1;
        uint8_t isTransparent   :1;
        uint8_t reservedFlags   :6;
        uint8_t lightOpacity;
        uint16_t pad0;
    };

    struct BlockRegistryTable {
       std::vector<SimulationAttributes>    simAttributes;
       std::vector<float>                   movementCosts;
       std::vector<std::array<int, 6>>      textureIndices;
       std::vector<glm::vec4>               colors;
       std::vector<std::string>             coldNames;
       std::vector<std::string>             generationLayers;
       std::unordered_map<std::string, VoxelTypeID, TransparentStringHash, std::equal_to<>> nameToIdMap;
       
       [[nodiscard]] inline size_t Size() const noexcept {
           return simAttributes.size();
       }
    };

    namespace BlockOps {
        [[nodiscard]] BlockRegistryTable LoadRegistryFromFile(std::string_view jsonPath) noexcept;
        
        [[nodiscard]] VoxelTypeID FindIdByString(const BlockRegistryTable& table, std::string_view internalName) noexcept;

        [[nodiscard]] VoxelTypeID FindIdByGenerationLayer(const BlockRegistryTable& table, std::string_view layerRole) noexcept;

        [[nodiscard]] inline bool IsSolid(const BlockRegistryTable& table, VoxelTypeID id) noexcept {
            const size_t idx = static_cast<size_t>(id);
            if (idx < table.simAttributes.size()) [[likely]] {
                return table.simAttributes[idx].isSolid != 0;
            }
            return false;
        }

        [[nodiscard]] inline bool IsTransparent(const BlockRegistryTable& table, VoxelTypeID id) noexcept {
            const size_t idx = static_cast<size_t>(id);
            if (idx < table.simAttributes.size()) [[likely]] {
                return table.simAttributes[idx].isTransparent != 0;
            }
            return true;
        }

        [[nodiscard]] inline float GetMovementCost(const BlockRegistryTable& table, VoxelTypeID id) noexcept {
            const size_t idx = static_cast<size_t>(id);
            if (idx < table.movementCosts.size()) [[likely]] {
                return table.movementCosts[idx];
            }
            return 1.0f;
        }

        [[nodiscard]] inline const std::array<int, 6>& GetTextureIndices(const BlockRegistryTable& table, VoxelTypeID id) noexcept {
            const size_t idx = static_cast<size_t>(id);
            if (idx < table.textureIndices.size()) [[likely]] {
                return table.textureIndices[idx];
            }
            return table.textureIndices[0];
        }

        [[nodiscard]] inline const glm::vec4& GetColor(const BlockRegistryTable& table, VoxelTypeID id) noexcept {
            const size_t idx = static_cast<size_t>(id);
            if (idx < table.colors.size()) [[likely]] {
                return table.colors[idx];
            }
            return table.colors[0];
        }
    }
}
