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

    alignas(16) struct BlockProperties {
        float movementCost{1.0f};
        bool isSolid{false};
        bool isTransparent{true}; 
        std::array<int, 6> textureIndices{0, 0, 0, 0, 0, 0};
        glm::vec4 color{0.0f, 0.0f, 0.0f, 0.0f};
    };

    struct BlockRegistryTable {
        std::vector<BlockProperties> records;
        std::vector<std::string> coldNames;
        std::unordered_map<std::string, VoxelTypeID, TransparentStringHash, std::equal_to<>> nameToIdMap;
    };

    namespace BlockOps {
        [[nodiscard]] BlockRegistryTable LoadRegistryFromFile(std::string_view jsonPath) noexcept;
        
        [[nodiscard]] VoxelTypeID FindIdByString(const BlockRegistryTable& table, std::string_view internalName) noexcept;

        [[nodiscard]] inline const BlockProperties& QueryProperties(const BlockRegistryTable& table, VoxelTypeID id) noexcept {
            const auto index = static_cast<size_t>(id);
            
            [[likely]] if (index < table.records.size()) {
                return table.records[index];
            }
            return table.records[0];
        }
    }
}