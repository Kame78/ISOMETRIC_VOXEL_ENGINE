
#include "../../include/render/ChunkRenderer.hpp"
#include "../../include/world/Chunk.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Render{
    void ChunkRenderer::Render(const std::vector<World::Chunk>& chunks,
                               const std::vector<ChunkRenderState>& renderStates,
                               const Shader& shader,
                               const Camera& camera) const {

        shader.Bind();

        shader.SetMat4("uProjection", camera.GetProjectionMatrix());
        shader.SetMat4("uView", camera.GetViewMatrix());

        for (size_t i = 0; i < chunks.size(); ++i) {
        //    if (renderStates[i].isVisible) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), chunks[i].worldPosition);
                shader.SetMat4("uModel", model);
                
                renderStates[i].mesh.Draw();
            }
        }
    }
