#include "../../include/world/BlockRegistry.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <algorithm>

namespace World {

    BlockRegistryTable BlockOps::LoadRegistryFromFile(std::string_view jsonPath) noexcept {
        BlockRegistryTable table;

        constexpr size_t PRE_ALLOCATION_TARGET = 512;
        table.simAttributes.reserve(PRE_ALLOCATION_TARGET);
        table.movementCosts.reserve(PRE_ALLOCATION_TARGET);
        table.textureIndices.reserve(PRE_ALLOCATION_TARGET);
        table.colors.reserve(PRE_ALLOCATION_TARGET);
        table.coldNames.reserve(PRE_ALLOCATION_TARGET);
        table.generationLayers.reserve(PRE_ALLOCATION_TARGET); // FIX: Pre-allocated memory channel

        // --- EXPLICIT INITIALIZATION FOR ID 0: ENGINE AIR FALLBACK ---
        table.simAttributes.push_back(SimulationAttributes{
            .isSolid = 0,
            .isTransparent = 1,
            .reservedFlags = 0,
            .lightOpacity = 0,
            .pad0 = 0
        });
        table.movementCosts.push_back(0.0f);
        table.textureIndices.push_back({-1, -1, -1, -1, -1, -1});
        table.colors.push_back(glm::vec4(0.0f));
        table.coldNames.emplace_back("engine:air");
        table.generationLayers.emplace_back("none");
        table.nameToIdMap.emplace("engine:air", static_cast<VoxelTypeID>(0));

        std::ifstream file((std::string(jsonPath)));
        if (!file.is_open()) {
            std::cerr << "[-] Error: Failed to resolve registry asset route: " << jsonPath << "\n";
            return table;
        }

        try {
            nlohmann::json root;
            file >> root;

            if (!root.contains("blocks") || !root["blocks"].is_array()) {
                std::cerr << "[-] Error: Target asset profile lacks structural block tracking arrays.\n";
                return table;
            }

            std::unordered_map<std::string, int> pathToTextureIdMap;

            for (const auto& item : root["blocks"]) {
                std::string stringId = item.value("string_id", "");
                if (stringId.empty()) [[unlikely]] {
                    std::cerr << "[!] Configuration Warning: Encountered anonymous block profile entry. Skipping line.\n";
                    continue;
                }

                if (table.nameToIdMap.contains(stringId)) [[unlikely]] {
                    std::cerr << "[!] Configuration Warning: Duplicate namespace identifier detected: " << stringId << ". Skipping row.\n";
                    continue;
                }

                // FIX: Corrected scoped brace placement making assignedId reachable
                const auto assignedId = static_cast<VoxelTypeID>(table.simAttributes.size());

                // --- STREAM POPULATION CHANNEL EXTRACTIONS ---
                const bool solidFlag = item.value("solid", true);
                const bool transFlag = item.value("transparent", false);
                const uint8_t opacity = transFlag ? static_cast<uint8_t>(item.value("light_opacity", 0)) : 255;

                table.simAttributes.push_back(SimulationAttributes{
                    .isSolid = static_cast<uint8_t>(solidFlag ? 1 : 0),
                    .isTransparent = static_cast<uint8_t>(transFlag ? 1 : 0),
                    .reservedFlags = 0,
                    .lightOpacity = opacity,
                    .pad0 = 0
                });

                table.movementCosts.push_back(item.value("movement_cost", 1.0f));
                table.generationLayers.push_back(item.value("generation_layer", "none"));

                glm::vec4 blockColor(1.0f);
                if (item.contains("color") && item["color"].is_array()) {
                    auto colArray = item["color"].get<std::vector<float>>();
                    if (colArray.size() >= 4) {
                        blockColor = glm::vec4(colArray[0], colArray[1], colArray[2], colArray[3]);
                    }
                }
                table.colors.push_back(blockColor);

                std::array<int, 6> texIndices{-1, -1, -1, -1, -1, -1};
                if (item.contains("textures") && item["textures"].is_array()) {
                    auto texArray = item["textures"].get<std::vector<std::string>>();
                    for (size_t i = 0; i < std::min(size_t(6), texArray.size()); ++i) {
                        const std::string& path = texArray[i];

                        if(path.empty() || path == "none" || path == "0") {
                            texIndices[i] = -1;
                            continue;
                        }

                        auto it = pathToTextureIdMap.find(path);
                        if (it != pathToTextureIdMap.end()) {
                            texIndices[i] = it->second;
                        } else {
                            auto newTex = std::make_unique<sf::Texture>();
                            if (newTex->loadFromFile(path)) {
                                newTex->setSmooth(true);
                                newTex->setRepeated(true);

                                int newIndex = static_cast<int>(table.loadedTextures.size());
                                pathToTextureIdMap[path] = newIndex;
                                texIndices[i] = newIndex;
                                table.loadedTextures.push_back(std::move(newTex));    
                            } else {
                                std::cerr << "[-] BlockRegistry Error: Failed to load file from path asset (texture): " << path << "\n";
                                texIndices[i] = -1;
                            }
                        }
                    }
                }
                table.textureIndices.push_back(texIndices);
                
                // Track cold properties completely decoupled from hot paths
                table.coldNames.push_back(stringId);
                table.nameToIdMap.insert_or_assign(std::move(stringId), assignedId);
            }
            std::cout << "[+] System Registry loaded successfully. Total Active Voxel Profiles: " << table.Size() << "\n";
        } 
        catch (const std::exception& e) {
            std::cerr << "[-] Exception caught parsing registry sheet: " << e.what() << "\n";
        }
        return table;
    }

    VoxelTypeID BlockOps::FindIdByString(const BlockRegistryTable& table, std::string_view internalName) noexcept {
        auto it = table.nameToIdMap.find(internalName);
        if (it != table.nameToIdMap.end()) {
            return it->second;
        }
        return static_cast<VoxelTypeID>(0);
    }

    VoxelTypeID BlockOps::FindIdByGenerationLayer(const BlockRegistryTable& table, std::string_view layerRole) noexcept {
        // Linear scan over cold text arrays is perfectly efficient because it runs outside performance-critical loops
        for (size_t i = 0; i < table.generationLayers.size(); ++i) {
            if (table.generationLayers[i] == layerRole) {
                return static_cast<VoxelTypeID>(i);
            }
        }
        return static_cast<VoxelTypeID>(0); // Fallback to engine:air if unassigned
    }

} // namespace World