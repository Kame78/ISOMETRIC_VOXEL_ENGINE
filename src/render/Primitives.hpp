#pragma once
#include "render/Mesh.hpp"
#include <vector>

namespace Primitives {
    inline std::vector<Vertex> GenerateCubeData() {
        // 1. Define the 24 unique vertex combinations pre-scaled to exactly 2.0 x 1.0 x 2.0
        // Width spans X [-1.0f to 1.0f], Height spans Y [-0.5f to 0.5f], Depth spans Z [-1.0f to 1.0f]
        const Vertex uniqueVertices[24] = {
            // === BACK FACE (Normal: 0, 0, -1) ===
            {{-1.0f, -0.5f, -1.0f}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}, glm::vec4(1.0f) },
            {{-1.0f,  0.5f, -1.0f}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}, glm::vec4(1.0f) },
            {{ 1.0f,  0.5f, -1.0f}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}, glm::vec4(1.0f) },
            {{ 1.0f, -0.5f, -1.0f}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}, glm::vec4(1.0f) },

            // === FRONT FACE (Normal: 0, 0, 1) ===
            {{-1.0f, -0.5f,  1.0f}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}, glm::vec4(1.0f) },
            {{ 1.0f, -0.5f,  1.0f}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}, glm::vec4(1.0f) },
            {{ 1.0f,  0.5f,  1.0f}, {0.0f,  0.0f,  1.0f}, {1.1f, 1.0f}, glm::vec4(1.0f) },
            {{-1.0f,  0.5f,  1.0f}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}, glm::vec4(1.0f) },

            // === LEFT FACE (Normal: -1, 0, 0) ===
            {{-1.0f, -0.5f, -1.0f}, {-1.0f, 0.0f,  0.0f}, {0.0f, 0.0f}, glm::vec4(1.0f) },
            {{-1.0f, -0.5f,  1.0f}, {-1.0f, 0.0f,  0.0f}, {1.0f, 0.0f}, glm::vec4(1.0f) },
            {{-1.0f,  0.5f,  1.0f}, {-1.0f, 0.0f,  0.0f}, {1.0f, 1.0f}, glm::vec4(1.0f) },
            {{-1.0f,  0.5f, -1.0f}, {-1.0f, 0.0f,  0.0f}, {0.0f, 1.0f}, glm::vec4(1.0f) },

            // === RIGHT FACE (Normal: 1, 0, 0) ===
            {{ 1.0f, -0.5f, -1.0f}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, glm::vec4(1.0f) },
            {{ 1.0f,  0.5f, -1.0f}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}, glm::vec4(1.0f) },
            {{ 1.0f,  0.5f,  1.0f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, glm::vec4(1.0f) },
            {{ 1.0f, -0.5f,  1.0f}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}, glm::vec4(1.0f) },

            // === BOTTOM FACE (Normal: 0, -1, 0) ===
            {{-1.0f, -0.5f, -1.0f}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}, glm::vec4(1.0f) },
            {{ 1.0f, -0.5f, -1.0f}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}, glm::vec4(1.0f) },
            {{ 1.0f, -0.5f,  1.0f}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}, glm::vec4(1.0f) },
            {{-1.0f, -0.5f,  1.0f}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}, glm::vec4(1.0f) },

            // === TOP FACE (Normal: 0, 1, 0) ===
            {{-1.0f,  0.5f,  1.0f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}, glm::vec4(1.0f) },
            {{ 1.0f,  0.5f,  1.0f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}, glm::vec4(1.0f) },
            {{ 1.0f,  0.5f, -1.0f}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}, glm::vec4(1.0f) },
            {{-1.0f,  0.5f, -1.0f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}, glm::vec4(1.0f) },
        };

        const unsigned int indices[36] = {
            0,  1,  2,    2,  3,  0,  // Back Face
            4,  5,  6,    6,  7,  4,  // Front Face
            8,  9,  10,   10, 11, 8,  // Left Face
            12, 13, 14,   14, 15, 12, // Right Face
            16, 17, 18,   18, 19, 16, // Bottom Face
            20, 21, 22,   22, 23, 20  // Top Face
        };

        std::vector<Vertex> builtTriangles;
        builtTriangles.reserve(36);
        for (unsigned int index : indices) {
            builtTriangles.push_back(uniqueVertices[index]);
        }
        return builtTriangles;
    }
}