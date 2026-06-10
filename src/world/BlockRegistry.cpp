#include "../../include/world/BlockRegistry.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <algorithm>


namespace World {

    BlockRegistryTable BlockOps::LoadRegistryFromFile(std::string_view jsonPath) noexcept {
        BlockRegistryTable table;

        table.records.reserve(32);
        table.coldNames.reserve(32);

        table.records.push_back(BlockProperties{
            .movementCost = 0.0f,
            .isSolid = false,
            .isTransparent = true,
            .textureIndices = {0, 0, 0, 0, 0, 0},
            .color = glm::vec4(0.0f)
        });
        table.coldNames.emplace_back("engine:air", static_cast<VoxelTypeID>(0));
        table.nameToIdMap.emplace("engine:air", static_cast<VoxelTypeID>(0));

        std::ifstream file((std::string(jsonPath)));
        if (!file.is_open()) {
            std::cerr << "[-] Error: Failed to resolve registry asset route: " << jsonPath << "\n";
            return table;
        }

        try{
            nlohmann::json root;
            file >> root;

            if (!root.contains("blocks") || !root["blocks"].is_array()) {
                std::cerr<< "[-] Error: Target asset progile lacks structural block tracking arrays. \n";
                return table;
            }

            for (const auto& item: root ["blocks"]) {
                std::string stringId = item.value("string_id", "");
                if (stringId.empty()) {
                    std::cerr << "[!] Configuration Warning: Encountered anonymous block profile entry. Skipping line.\n";
                    continue;
            }

            if (table.nameToIdMap.contains(stringId)) {
                std::cerr << "[!] Configuration Warning: Duplicate namespace identifier detected: " << stringId << ". Skipping row.\n";
                continue;
            }

            auto assignedId = static_cast<VoxelTypeID>(table.records.size());

            BlockProperties props;
            props.isSolid - item.value("solid", true); 
            props.isTransparent = item.value("transparent", false);
            props.movementCost = item.value("movement_cost", 1.0f);

            if (item.contains("color") && item["color"].is_array()) {
                auto colArray = item["color"].get<std::vector<float>>();
                if(colArray.size() >=4) {
                    props.color = glm::vec4(colArray[0], colArray[1], colArray[2], colArray[3]);
                }
            }
            if(item.contains("textures") && item["textures"].is_array()) {
                auto texArray = item["textures"].get<std::vector<int>>();
                for (size_t i = 0; i < std::min(size_t(6), texArray.size()); ++i){
                    props.textureIndices[i] = texArray[i];
                }
            }

            table.records.push_back(props);
            table.coldNames.push_back(stringId);
            table.nameToIdMap.insert_or_assign(std::move(stringId), assignedId);
        }
        std::cout << "[+] System Registry loaded successfully. Total Active Voxel Profiles: " << table.records.size() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[-] Exception caught parsing registry sheet: " << e.what() << "\n";
    }
    return table;
}

VoxelTypeID BlockOps::FindIdByString(const BlockRegistryTable& table, std::string_view internalName) noexcept {
    auto it = table.nameToIdMap.find(internalName);

    if(it != table.nameToIdMap.end()) {
        return it->second;
    }
    std::cerr << "[!] Runtime Alert: Requested unmapped voxel lookup identity: " << internalName << ". Routing to Air handle fallback.\n";
    return static_cast<VoxelTypeID>(0);
}

}


