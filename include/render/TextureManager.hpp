#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>

namespace Render {

    class TextureManager {
    public:
        TextureManager() = default;
        ~TextureManager() = default;

        [[nodiscard]] uint32_t CreateTextureArray(const std::vector<std::string>& filePath, int width = 256, int height = 256) noexcept;

        void Clear() noexcept {
            if (m_globalBlockArrayID != 0) {
                glDeleteTextures(1, &m_globalBlockArrayID);
                m_globalBlockArrayID = 0;
            }
        }
    private:
        uint32_t m_globalBlockArrayID = 0;
    
};

}