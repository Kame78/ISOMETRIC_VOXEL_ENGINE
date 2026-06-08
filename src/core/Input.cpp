#include "core/Input.hpp"

std::unordered_map<sf::Keyboard::Key, bool> Input::s_currentKeyStates;
std::unordered_map<sf::Keyboard::Key, bool> Input::s_previousKeyStates;

void Input::UpdateStates() noexcept {
    s_previousKeyStates = s_currentKeyStates;
}

void Input::RecordKeyPress(sf::Keyboard::Key key, bool isPressed) noexcept {
    s_currentKeyStates[key] = isPressed;
}

bool Input::IsKeyHeld(sf::Keyboard::Key key) noexcept {
    return sf::Keyboard::isKeyPressed(key);
}

bool Input::IsKeyDown(sf::Keyboard::Key key) noexcept {
    auto itCurrent = s_currentKeyStates.find(key);
    bool currentlyPressed = (itCurrent != s_currentKeyStates.end()) ? itCurrent->second : false;
    auto itPrevious = s_previousKeyStates.find(key);
    bool previousPressed = (itPrevious != s_previousKeyStates.end()) ? itPrevious->second : false;

    return currentlyPressed && !previousPressed;

}