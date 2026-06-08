#pragma once
#include <SFML/Window.hpp>
#include <unordered_map>

class Input {
    private:

    static std::unordered_map<sf::Keyboard::Key, bool> s_currentKeyStates;

    static std::unordered_map<sf::Keyboard::Key, bool> s_previousKeyStates;

    public:
    
    Input() = delete;

    static void UpdateStates() noexcept;
    static void RecordKeyPress(sf::Keyboard::Key key, bool isPressed) noexcept;

    static bool IsKeyHeld(sf::Keyboard::Key key) noexcept;
    static bool IsKeyDown(sf::Keyboard::Key key) noexcept;
};