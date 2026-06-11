
#include "../../include/render/ChunkRenderer.hpp"
#include "../../include/world/Chunk.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Render{
    void ChunkRenderer::Render(const std::vector<ChunkRenderState>& renderStates,
                               const std::vector<glm::vec3>& chunkPositions,
                               const Shader& shader,
                               const Camera& camera) const noexcept {

        
        shader.Bind();
        shader.SetMat4("uProjection", camera.GetProjectionMatrix());
        shader.SetMat4("uView", camera.GetViewMatrix());

        for (size_t i = 0; i < renderStates.size(); ++i) {
        //    if (renderStates[i].isVisible) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), chunkPositions[i]);
                shader.SetMat4("uModel", model);
                
                renderStates[i].mesh.Draw();
            }
        }
    }
