#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstddef>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec4 color;
};

class Mesh{
private:
    unsigned int m_VAO{ 0 };
    unsigned int m_VBO{ 0 };
    GLsizei m_vertexCount{ 0 };

    void SetupMesh(const std::vector<Vertex>& vertices);

public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void Create(const std::vector<Vertex> & verticies);
    void Draw() const;
    void Destroy();
};
