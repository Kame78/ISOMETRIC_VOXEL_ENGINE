#include "../../include/render/TextureManager.hpp"
#include <SFML/Graphics/Image.hpp>
#include <iostream>

namespace Render {

    uint32_t TextureManager::CreateTextureArray(const std::vector<std::string>& filePath, int width, int height) noexcept {
        int layerCount = static_cast<int>(filePath.size());
        if (layerCount == 0) return 0;

        uint32_t arrayID = 0;
        glGenTextures(1, &arrayID);
        glBindTexture(GL_TEXTURE_2D_ARRAY, arrayID);

        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, layerCount, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        for (int i = 0; i < layerCount; ++i) {
            sf::Image img;
            if(img.loadFromFile(filePath[i])) {
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
                                static_cast<GLsizei>(img.getSize().x),
                                static_cast<GLsizei>(img.getSize().y),
                            1, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
            } else {
                std::cerr << "[-] TextureManager Error: Failed to open file: " << filePath[i] << "\n";
            }
        }
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        return arrayID;
    }  
}