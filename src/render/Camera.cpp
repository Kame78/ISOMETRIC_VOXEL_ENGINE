#include "../../include/render//Camera.hpp"
#include "../../include/core/Input.hpp"
#include<glm/gtc/matrix_transform.hpp>
#include <algorithm>

Camera::Camera(float width, float height) {
    UpdateViewport(width, height);
}

void Camera::UpdateViewport(float width, float height) {
    if(height > 0.0f) {
        m_aspectRatio = width / height;
    }
    UpdateMatrices();
}

void Camera::ProcessInput() noexcept {
    float camSpeed = 0.04f;
    float zoomSpeed = 0.01f;

    if (Input::IsKeyHeld(sf::Keyboard::Key::A)) Move(glm::vec3(-camSpeed, 0.0f, -camSpeed));
    if (Input::IsKeyHeld(sf::Keyboard::Key::D)) Move(glm::vec3(camSpeed, 0.0f, camSpeed));
    if (Input::IsKeyHeld(sf::Keyboard::Key::S)) Move(glm::vec3(-camSpeed, 0.0f, camSpeed));
    if (Input::IsKeyHeld(sf::Keyboard::Key::W)) Move(glm::vec3(camSpeed, 0.0f, -camSpeed));

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
    m_zoom = std::clamp(m_zoom + amount, 0.1f, 10.0f);
    UpdateMatrices();
}

void Camera::UpdateMatrices() {
    float extentY = 3.0f * m_zoom;
    float extentX = extentY * m_aspectRatio;
    m_projectionMatrix = glm::ortho(-extentX, extentX, -extentY, extentY, -100.0f, 1000.0f);

    glm::mat4 isometricOrientation = glm::mat4(1.0f);

    isometricOrientation = glm::rotate(isometricOrientation, glm::radians(35.264f), glm::vec3(1.0f, 0.0f, 0.0f));

    isometricOrientation = glm::rotate(isometricOrientation, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), -m_target);

    m_viewMatrix = isometricOrientation * translation;
}