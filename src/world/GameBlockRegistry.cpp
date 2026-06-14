#include "../../include/world/GameBlockRegistry.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

namespace Game {

    bool BlockConfigRegistry::LoadGameAssets(
        std::string_view jsonPath,
        Render::VoxelCoreRegistry& targetCore,
        std::vector<std::string>& outUniqueTexturePaths) noexcept {
            std::ifstream file ((std::string(jsonPath)));
            if(!file.is_open()) return false;

            try {
                nlohmann::json root;
                file >> root;

                constexpr size_t INITIAL_RESERVE = 128;
                m_coldNames.reserve(INITIAL_RESERVE);
                m_generationLayers.reserve(INITIAL_RESERVE);

                targetCore.simAttributes.reserve(INITIAL_RESERVE);
                targetCore.textureIndices.reserve(INITIAL_RESERVE);
                targetCore.colors.reserve(INITIAL_RESERVE);
                targetCore.meshTopologies.reserve(INITIAL_RESERVE);

                targetCore.simAttributes.push_back({.isSolid = 0, .isTransparent = 1, .reserved = 0, .lightOpacity = 0, .padding = 0});
                targetCore.textureIndices.push_back({-1, -1, -1, -1, -1, -1});
                targetCore.colors.push_back(glm::vec4(0.0f));
                targetCore.meshTopologies.push_back(Render::MeshTopology::Air);
                m_coldNames.emplace_back("engine::air");
                m_generationLayers.emplace_back("none");
                m_nameToIdMap.emplace("engine::air", 0);

                std::vector<std::array<std::string, 6>> tempTextureStrings;
                tempTextureStrings.push_back({"none", "none", "none", "none", "none", "none"});

                if (root.contains("terrain_blocks") && root["terrain_blocks"].is_array()) {
                    ParseAssetArray(root["terrain_blocks"], targetCore, tempTextureStrings, outUniqueTexturePaths);
                }

                if (root.contains("procedural_decorations") && root["procedural_decorations"].is_array()) {
                    ParseAssetArray(root["procedural_decorations"], targetCore, tempTextureStrings, outUniqueTexturePaths);
                }

                targetCore.textureIndices.resize(targetCore.Size());
                for(size_t i = 0; i < targetCore.Size(); ++i) {
                    for(size_t face = 0; face < 6; ++face) {
                        const std::string& path = tempTextureStrings[i][face];
                        if(path == "none") {
                            targetCore.textureIndices[i][face] = -1;
                        } else {
                            auto it = std::find(outUniqueTexturePaths.begin(), outUniqueTexturePaths.end(), path);
                            targetCore.textureIndices[i][face] = static_cast<int>(std::distance(outUniqueTexturePaths.begin(), it));
                        }
                    }
                }
                return true;
            }
            catch (const std::exception& e) {
                std::cerr << "[-] Game Asset parsing fault: " << e.what() << "\n";
                return false;
            }
        }

        void BlockConfigRegistry::ParseAssetArray(
            const nlohmann::json& assetArray,
            Render::VoxelCoreRegistry& targetCore,
            std::vector<std::array<std::string, 6>>& tempTextureStrings,
            std::vector<std::string>& outUniqueTexturePaths) noexcept
        {
            for (const auto& node : assetArray) {
                std::string sId = node.value("string_id", "");
                if(sId.empty() || m_nameToIdMap.contains(sId)) continue;

                const auto assignedId = static_cast<Render::VoxelTypeID>(targetCore.Size());

                const bool solid = node.value("solid", true);
                const bool trans = node.value("transparent", false);
                targetCore.simAttributes.push_back({
                    .isSolid = static_cast<uint8_t>(solid ? 1 : 0),
                    .isTransparent = static_cast<uint8_t>(trans ? 1 : 0),
                    .reserved = 0,
                    .lightOpacity = trans ? static_cast<uint8_t>(node.value("light_opacity", 0)) : uint8_t(255),
                    .padding = 0
                });
                std::string topo = node.value("mesh_topology", "cube");
                Render::MeshTopology meshTopo = Render::MeshTopology::OpaqueCube;
                if (topo == "cross_quad") meshTopo = Render::MeshTopology::CrossQuad;
                else if (topo == "fluid") meshTopo = Render::MeshTopology::Fluid;
                else if (topo == "air") meshTopo = Render::MeshTopology::Air;
                targetCore.meshTopologies.push_back(meshTopo);

                glm::vec4 col (1.0f);
                if (node.contains("color")) {
                    auto c = node["color"].get<std::vector<float>>();
                    if (c.size() >=4) col = glm::vec4(c[0], c[1], c[2], c[3]);
                }
                targetCore.colors.push_back(col);

                std::array<std::string, 6> blockTextures{"none", "none", "none", "none", "none", "none"};
                if (node.contains("textures")) {
                    auto tArr = node["textures"].get<std::vector<std::string>>();
                    for (size_t i = 0; i < std::min(size_t(6), tArr.size()); ++i) {
                        if (!tArr[i].empty() && tArr[i] != "none") {
                            blockTextures[i] = tArr[i];
                            if(std::find(outUniqueTexturePaths.begin(), outUniqueTexturePaths.end(), tArr[i]) == outUniqueTexturePaths.end()) {    
                            outUniqueTexturePaths.push_back(tArr[i]);
                        }
                    }        
                }
            }
            tempTextureStrings.push_back(blockTextures);

            m_coldNames.push_back(sId);
            m_generationLayers.push_back(node.value("generation_layer", "none"));
            m_nameToIdMap.insert_or_assign(std::move(sId), assignedId);

            if (node.contains("decoration_rules")) {
                const auto& r = node["decoration_rules"];
                m_decorationRules.push_back(World::Systems::DecorationRule{
                    .voxelId = assignedId,
                    .minHeight = r.value("min_height", int16_t(0)),
                    .maxHeight = r.value("max_height", int16_t(255)),
                    .minLandMask = r.value("min_land_mask", 0.2f),
                    .spawnChance = r.value("spawn_chance", 0.02f),
                    .hashSalt = r.value("hash_salt", uint16_t(assignedId * 137))
                });
            }
        }
    }
    Render::VoxelTypeID BlockConfigRegistry::FindIdByString(std::string_view internalName) const noexcept {
        if (auto it = m_nameToIdMap.find(internalName); it != m_nameToIdMap.end()) return it->second;
        return 0;
    }
    Render::VoxelTypeID BlockConfigRegistry::FindIdByGenerationLayer(std::string_view layerRole) const noexcept {
        for (size_t i = 0; i < m_generationLayers.size(); ++i) {
            if (m_generationLayers[i] == layerRole) return static_cast<Render::VoxelTypeID>(i);
        }
        return 0;
    }
}
    
