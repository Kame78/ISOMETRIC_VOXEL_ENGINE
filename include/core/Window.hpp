#pragma once
#include <SFML/Graphics.hpp>
#include <string_view>
#include <glm/glm.hpp>
#include <SFML/Window.hpp>



class Window {
private:
    bool m_isOpen{ false };

    void HandleEvents();

    sf::RenderWindow m_window;
public:
    Window() = default;
    ~Window() = default;

    bool Create(unsigned int width, unsigned int height, std::string_view title);

    void ProcessFrame();
    void Display();
    void Close();

    bool IsOpen() const noexcept {return m_window.isOpen();}
    glm::vec2 GetSize() const noexcept;

    [[nodiscard]] sf::RenderWindow& GetSFMLWindow() noexcept { return m_window; }
};