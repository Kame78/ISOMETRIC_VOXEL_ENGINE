#pragma once
#include <glm/glm.hpp>

class Camera {
private:
    glm::vec3 m_target{ 0.0f, 10.0f, 96.0f};
    float m_zoom{ 15.0f };
    float m_aspectRatio{1.7777f};

    glm::mat4 m_viewMatrix{1.0f};
    glm::mat4 m_projectionMatrix{1.0f};

    void UpdateMatrices();

public:
    Camera(float width, float height);
    ~Camera() = default;

    void UpdateViewport(float width, float height);

    void ProcessInput() noexcept;

    void Move(const glm::vec3& delta);
    void SetTarget(const glm::vec3& target);
    void AdjustZoom(float amount);

    glm::mat4 GetViewMatrix() const noexcept { return m_viewMatrix; }
    glm::mat4 GetProjectionMatrix() const noexcept { return m_projectionMatrix; }
    glm::vec3 GetTarget() const noexcept { return m_target; }

};