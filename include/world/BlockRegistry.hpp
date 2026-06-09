#pragma once

#include <string>
#include <array>
#include <glm/glm.hpp>
#include "Chunk.hpp"

namespace World {

    struct BlockProperties {
        std::string name{"Air"};
        bool isSolid{false};
        bool isTransparent{true}; 
    
        glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
        std::array<int, 6> textureIndices{0, 0, 0, 0, 0, 0};
    
        float movementCost{1.0f};
    };

    class BlockRegistry {
    public:

        BlockRegistry() = delete;

        static void Initialize() noexcept;

        static const BlockProperties& GetProperties(BlockID id) noexcept {
            auto index = static_cast<size_t>(id);
            if(index >= m_registryTable.size()) {
                return m_registryTable[0];
            }
            return m_registryTable[index];
        
            }

        private:
            static std::array<BlockProperties, 4> m_registryTable;
    };
}