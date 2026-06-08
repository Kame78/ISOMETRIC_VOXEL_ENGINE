#include "../../include/render/Mesh.hpp"
#include <utility>

Mesh::~Mesh() {
    Destroy();
}

Mesh::Mesh(Mesh&& other) noexcept 
    : m_VAO(other.m_VAO), m_VBO(other.m_VBO), m_vertexCount(other.m_vertexCount) {
    other.m_VAO = 0;
    other.m_VBO = 0;
    other.m_vertexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        Destroy();
        m_VBO = other.m_VBO;
        m_VAO = other.m_VAO;
        m_vertexCount = other.m_vertexCount;

        other.m_VAO = 0;
        other.m_VBO = 0;
        other.m_vertexCount = 0;
    }
    return *this;
}

void Mesh::Create(const std::vector<Vertex>& vertices) {
    Destroy();
    m_vertexCount = static_cast<GLsizei>(vertices.size());
    SetupMesh(vertices);
}

void Mesh::SetupMesh(const std::vector<Vertex>& vertices) {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::Draw() const {
    if (m_VAO != 0) {
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
        glBindVertexArray(0);
    }
}

void Mesh::Destroy() {
    if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);
    if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
    m_VBO = 0;
    m_VAO = 0;
    m_vertexCount = 0;
}