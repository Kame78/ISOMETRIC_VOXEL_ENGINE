#include "core/Window.hpp"
#include <glad/glad.h>
#include "core/Input.hpp"
#include <iostream>
#include <optional>

bool Window::Create(unsigned int width, unsigned int height, std::string_view title) {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Default;

    m_window.create(sf::VideoMode({width, height}), title.data(), sf::State::Windowed, settings);
    m_window.setVerticalSyncEnabled(true);

    if (!gladLoadGL()) {
        std::cerr << "[-] Error: GLAD initialization failed inside Window Wrapper!\n";
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    return true;
}

void Window::ProcessFrame() {
    HandleEvents();
}

void Window::HandleEvents() {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
        else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
            glViewport(0, 0, resized->size.x, resized->size.y);
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            Input::RecordKeyPress(keyPressed->code, true);
        }
        else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            Input::RecordKeyPress(keyReleased->code, false);
        }
    }
}

void Window::Display() {
    m_window.display();
}

void Window::Close() {
    m_window.close();
}

glm::vec2 Window::GetSize() const noexcept {
    auto size = m_window.getSize();
    return glm::vec2(static_cast<float>(size.x), static_cast<float>(size.y));
}