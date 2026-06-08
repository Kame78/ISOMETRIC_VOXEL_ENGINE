#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "render/Shader.hpp"
#include "render/Camera.hpp"
#include "render/Mesh.hpp"

class Renderer {
private:
    glm::vec4 m_clearColor{ 0.12f, 0.13f, 0.18, 1.0f };

public:
    Renderer() = default;
    ~Renderer() = default;

void BeginFrame() const noexcept;
void DrawMesh(const Mesh& mesh, const Shader& shader, const Camera& camera, const glm::mat4& modelMatrix) const noexcept;

void SetClearColor(const glm::vec4& color) noexcept { m_clearColor = color;}
};