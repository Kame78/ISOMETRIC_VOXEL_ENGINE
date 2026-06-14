#pragma once
#include "../render/VoxelCoreRegistry.hpp"
#include "../world/WorldDecoratorSystem.hpp"
#include <string>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <span>
#include <nlohmann/json.hpp>

namespace Game {

    struct TransparentStringHash {
        using is_transparent = void;
        [[nodiscard]] size_t operator()(std::string_view sv) const noexcept {return std::hash<std::string_view>{}(sv);}
        [[nodiscard]] size_t operator()(const std::string& s) const noexcept {return std::hash<std::string>{}(s);}
    };

    class BlockConfigRegistry {
    public:
        BlockConfigRegistry() = default;

        bool LoadGameAssets(
            std::string_view jsonPath,
            Render::VoxelCoreRegistry& targetCore,
            std::vector<std::string>& outUniqueTexturePaths) noexcept;

        [[nodiscard]] Render::VoxelTypeID FindIdByString(std::string_view internalName) const noexcept;
        [[nodiscard]] Render::VoxelTypeID FindIdByGenerationLayer(std::string_view layerRole) const noexcept;
        [[nodiscard]] std::span<const World::Systems::DecorationRule> GetDecorationRules() const noexcept {
            return std::span<const World::Systems::DecorationRule>{m_decorationRules.data(), m_decorationRules.size()};
        }

    private:

            void ParseAssetArray(
                const nlohmann::json& assetArray,
                Render::VoxelCoreRegistry& targetCore,
                std::vector<std::array<std::string, 6>>& tempTextureStrings,
                std::vector<std::string>& outUniqueTexturePaths) noexcept;

            std::vector<std::string> m_coldNames;
            std::vector<std::string> m_generationLayers;
            std::vector<World::Systems::DecorationRule> m_decorationRules;
            std::unordered_map<std::string, Render::VoxelTypeID, TransparentStringHash, std::equal_to<>> m_nameToIdMap;       
    };
}