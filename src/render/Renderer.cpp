#include "../../include/render/Renderer.hpp"

void Renderer::BeginFrame() const noexcept {

    glClearColor(1.0f, 0.0f, 0.3f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void Renderer::DrawMesh(const Mesh& mesh, const Shader& shader, const Camera& camera, const glm::mat4& modelMatrix) const noexcept {
    shader.Bind();

    shader.SetMat4("uProjection", camera.GetProjectionMatrix());
    shader.SetMat4("uView", camera.GetViewMatrix());
    shader.SetMat4("uModel", modelMatrix);

    shader.SetBool("uUseTexture", false);

    mesh.Draw();
}

