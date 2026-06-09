#include "../../include/world/BlockRegistry.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <vector>

namespace World {
std::array<BlockProperties, 4> BlockRegistry::m_registryTable;

void BlockRegistry::Initialize() noexcept {
     auto& air = m_registryTable[static_cast<size_t>(BlockID::AIR)];
        air.name = "Air",
        air.isSolid = false,
        air.isTransparent = true,
        air.color = glm::vec4(0.0f, 0.0f, 0.0f , 0.0f),
        air.textureIndices = {0, 0, 0, 0, 0, 0,},
        air.movementCost = 0.0f;
    
        std::ifstream file ("assets/data/blocks.json");
        if(!file.is_open()) {
            std::cerr << "CRITICAL ERROR: Failed to locate blocks.json !" << std::endl;
            std::cerr << "Engine hardcoded fallbacks deployed." << std::endl;
        
            // === Hardcoded Fallbacks (Used if file loading fails) ===
        
            auto& dirt = m_registryTable[static_cast<size_t>(BlockID::DIRT)];
            dirt.name           = "Dirt";
            dirt.isSolid        = true;
            dirt.isTransparent  = false;
            dirt.color          = glm::vec4(0.45f, 0.34f, 0.22f, 1.0f);
            dirt.textureIndices = {2, 2, 2, 2, 2, 2};
            dirt.movementCost   = 1.0f;

            auto& stone = m_registryTable[static_cast<size_t>(BlockID::STONE)];
            stone.name           = "Stone";
            stone.isSolid        = true;
            stone.isTransparent  = false;
            stone.color          = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
            stone.textureIndices = {3, 3, 3, 3, 3, 3};
            stone.movementCost   = 1.5f;

            auto& grass = m_registryTable[static_cast<size_t>(BlockID::GRASS)];
            grass.name           = "Grass";
            grass.isSolid        = true;
            grass.isTransparent  = false;
            grass.color          = glm::vec4(0.32f, 0.51f, 0.23f, 1.0f);
            grass.textureIndices = {0, 2, 1, 1, 1, 1};
            grass.movementCost   = 1.0f;
        
            return;
        }

        try {
            nlohmann::json data;
            file >> data;

            if (!data.contains("blocks") || !data["blocks"].is_array()) {
            std::cerr << "ERROR: 'blocks' array missing from JSON root node!" << std::endl;
            return;
        }

            for (const auto& item : data["blocks"]) {

                if (!item.contains("id")) {
                std::cerr << "WARNING: Skipping a JSON block entry because it is missing an 'id' key!" << std::endl;
                continue;
            }

                size_t idIndex =item["id"].get<size_t>();
                if (idIndex >= m_registryTable.size()) continue;

                auto& props = m_registryTable[idIndex];

                props.name           = item.value("name", "Unknown Voxel");
                props.isSolid        = item.value("solid", true);
                props.isTransparent  = item.value("transparent", false);
                props.movementCost   = item.value("movementCost", 1.0f);

              if (item.contains("color") && item["color"].is_array()) {  
                auto colorArray       = item["color"].get<std::vector<float>>();
                if (colorArray.size() >= 4) {
                props.color = glm::vec4(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);   
                }
              } else {
                props.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
              }           
                if (item.contains("textures") && item["textures"].is_array()) {
                    auto texArray = item["textures"].get<std::vector<int>>();
                    if (texArray.size() >= 6) {
                        for(size_t i = 0; i < 6; ++i) {
                            props.textureIndices[i] = texArray[i];
                        }
                    }
                }
            }
        }

        catch (const nlohmann::json::exception& e) {
            std::cerr << "JSON Parser Exception: " << e.what() << std::endl;
    
        }
    }
}

