#pragma once
#include "render/Mesh.hpp"
#include <vector>

namespace Primitives {
    inline std::vector<Vertex> GenerateCubeData() {
        // 1. Define the 24 unique vertex combinations needed for hard, flat edges
        const Vertex uniqueVertices[24] = {
            // === BACK FACE (Normal: 0, 0, -1) ===
            {{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, // 0
            {{-0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}}, // 1
            {{ 0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, // 2
            {{ 0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}}, // 3

            // === FRONT FACE (Normal: 0, 0, 1) ===
            {{-0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // 4
            {{ 0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, // 5
            {{ 0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // 6
            {{-0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, // 7

            // === LEFT FACE (Normal: -1, 0, 0) ===
            {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f,  0.0f}, {0.0f, 0.0f}}, // 8
            {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f,  0.0f}, {1.0f, 0.0f}}, // 9
            {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f,  0.0f}, {1.0f, 1.0f}}, // 10
            {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f,  0.0f}, {0.0f, 1.0f}}, // 11

            // === RIGHT FACE (Normal: 1, 0, 0) ===
            {{ 0.5f, -0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // 12
            {{ 0.5f,  0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // 13
            {{ 0.5f,  0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // 14
            {{ 0.5f, -0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // 15

            // === BOTTOM FACE (Normal: 0, -1, 0) ===
            {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}}, // 16
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}, // 17
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}}, // 18
            {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}}, // 19

            // === TOP FACE (Normal: 0, 1, 0) ===
            {{-0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}, // 20
            {{ 0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}}, // 21
            {{ 0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, // 22
            {{-0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}  // 23
        };

        // 2. Map the indices out using your triangle design layout logic
        const unsigned int indices[36] = {
            0,  1,  2,    2,  3,  0,  // Back Face
            4,  5,  6,    6,  7,  4,  // Front Face
            8,  9,  10,   10, 11, 8,  // Left Face
            12, 13, 14,   14, 15, 12, // Right Face
            16, 17, 18,   18, 19, 16, // Bottom Face
            20, 21, 22,   22, 23, 20  // Top Face
        };

        // 3. Let C++ unroll the index loop array list into your final mesh container
        std::vector<Vertex> builtTriangles;
        builtTriangles.reserve(36);

        for (unsigned int index : indices) {
            builtTriangles.push_back(uniqueVertices[index]);
        }

        return builtTriangles;
    }
}