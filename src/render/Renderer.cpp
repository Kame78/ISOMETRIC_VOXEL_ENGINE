#include "../../include/render/Renderer.hpp"

void Renderer::BeginFrame() const noexcept {

}

void Renderer::DrawMesh(const Mesh& mesh, const Shader& shader, const Camera& camera, const glm::mat4& modelMatrix) const noexcept {
    shader.Bind();

    shader.SetMat4("uProjection", camera.GetProjectionMatrix());
    shader.SetMat4("uView", camera.GetViewMatrix());
    shader.SetMat4("uModel", modelMatrix);

    shader.SetBool("uUseTexture", false);

    mesh.Draw();
}

