#pragma once
#include <SFML/Window.hpp>
#include <string_view>
#include <glm/glm.hpp>

class Window {
private:
    sf::Window m_window;
    bool m_isOpen{ false };

    void HandleEvents();

public:
    Window() = default;
    ~Window() = default;

    bool Create(unsigned int width, unsigned int height, std::string_view title);

    void ProcessFrame();
    void Display();
    void Close();

    bool IsOpen() const noexcept {return m_window.isOpen();}
    glm::vec2 GetSize() const noexcept;
};