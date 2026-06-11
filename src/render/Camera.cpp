#include "../../include/render/Camera.hpp"
#include "../../include/core/Input.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

Camera::Camera(float width, float height) 
    : m_target(96.f, 10.f, 96.f),
    m_zoom(15.f),
    m_aspectRatio(1.7777f)
{
    UpdateViewport(width, height);
}

void Camera::UpdateViewport(float width, float height) {
    if(height > 0.0f) {
        m_aspectRatio = width / height;
    }
    UpdateMatrices();
}

void Camera::ProcessInput() noexcept {
    // Re-tuned speed constants to keep movement smooth across lookAt vectors
    float camSpeed = 0.55f; 
    float zoomSpeed = 0.04f;

    if (Input::IsKeyHeld(sf::Keyboard::Key::A)) Move(glm::vec3(-camSpeed, 0.0f, camSpeed));
    if (Input::IsKeyHeld(sf::Keyboard::Key::D)) Move(glm::vec3(camSpeed, 0.0f, -camSpeed));
    if (Input::IsKeyHeld(sf::Keyboard::Key::S)) Move(glm::vec3(camSpeed, 0.0f, camSpeed));
    if (Input::IsKeyHeld(sf::Keyboard::Key::W)) Move(glm::vec3(-camSpeed, 0.0f, -camSpeed));

    if(Input::IsKeyHeld(sf::Keyboard::Key::Up)) AdjustZoom(zoomSpeed);
    if(Input::IsKeyHeld(sf::Keyboard::Key::Down)) AdjustZoom(-zoomSpeed);
}

void Camera::Move(const glm::vec3& delta) {
    m_target += delta;
    UpdateMatrices();
}

void Camera::SetTarget(const glm::vec3& target) {
    m_target = target;
    UpdateMatrices();
}

void Camera::AdjustZoom(float amount) {
    m_zoom = std::clamp(m_zoom + amount, 1.0f, 100.0f);
    UpdateMatrices();
}

void Camera::UpdateMatrices() {
    float extentY = 3.0f * m_zoom;
    float extentX = extentY * m_aspectRatio;
    
    // 🔑 FIXED: Reduce the clipping volume to match the island scale.
    // 0.1f to 500.0f provides much higher sorting precision.
    m_projectionMatrix = glm::ortho(-extentX, extentX, -extentY, extentY, 0.1f, 1500.0f);

    float camDistance = 250.0f; // Closer camera distance = better precision
    glm::vec3 cameraPosition = m_target + glm::vec3(camDistance, camDistance * 0.8164f, camDistance);

    m_viewMatrix = glm::lookAt(cameraPosition, m_target, glm::vec3(0.0f, 1.0f, 0.0f));
}