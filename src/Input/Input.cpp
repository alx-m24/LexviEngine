#include "pch.h"

#include "Input/Input.hpp"

using namespace Lexvi;

Lexvi::Input::KeyState Input::getKeyState(int key) const
{
    auto it = keys.find(key);
    if (it == keys.end()) {
        return {false, false, false};
    }

    return it->second;
}

Lexvi::Input::MouseButtonState Lexvi::Input::getMouseButtonState(int button) const
{
    auto it = mouseButtons.find(button);
    if (it == mouseButtons.end()) {
        return { false, false, false };
    }

    return it->second;
}

void Lexvi::Input::Update()
{
    mouseDelta = glm::vec2(0.0f);
    lastmousePos = mousePos;
    scrollDeltaY = 0.0f;

    for (auto& key_state_pair : keys) {
        key_state_pair.second.wasPressed = key_state_pair.second.isPressed;
        key_state_pair.second.isHeld = false;
    }

    for (auto& button_state_pair : mouseButtons) {
        button_state_pair.second.wasPressed = button_state_pair.second.isPressed;
    }

    glfwPollEvents();
}

bool Input::isKeyHeld(int key) const
{
    const KeyState state = getKeyState(key);
    return state.isPressed && state.wasPressed;
}

bool Input::isKeyRepeat(int key) const
{
    const KeyState state = getKeyState(key);
    return state.isHeld;
}

bool Input::isKeyReleased(int key) const
{
    const KeyState state = getKeyState(key);
    return !state.isPressed && state.wasPressed;
}

bool Input::isKeyStartedHold(int key) const
{
    const KeyState state = getKeyState(key);
    return state.isPressed && !state.wasPressed;
}

bool Lexvi::Input::isMouseButtonPressed(int button) const
{
    MouseButtonState state = getMouseButtonState(button);
    return state.isPressed && !state.wasPressed;
}

bool Lexvi::Input::isMouseButtonHeld(int button) const
{
    MouseButtonState state = getMouseButtonState(button);
    return state.isPressed && state.wasPressed;
}

bool Lexvi::Input::isMouseButtonReleased(int button) const
{
    MouseButtonState state = getMouseButtonState(button);
    return !state.isPressed && state.wasPressed;
}

glm::vec2 Lexvi::Input::getMousePosition() const
{
    return mousePos;
}

glm::vec2 Lexvi::Input::getMouseDelta() const
{
    return mouseDelta;
}

float Lexvi::Input::getScrollWheelDelta() const
{
    return scrollDeltaY;
}

unsigned int Lexvi::Input::getScreenWidth() const
{
    return SCR_WIDTH;
}

unsigned int Lexvi::Input::getScreenHeight() const
{
    return SCR_HEIGHT;
}

void Lexvi::Input::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Lexvi::Input* input = reinterpret_cast<Lexvi::Input*>(glfwGetWindowUserPointer(window));
    if (!input) return;

    input->SCR_WIDTH = width;
    input->SCR_HEIGHT = height;

    ResizeEvent e{ width, height };
    for (auto& cb : input->resizeCallbacks) cb(e);
}

void Lexvi::Input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Lexvi::Input* input = reinterpret_cast<Lexvi::Input*>(glfwGetWindowUserPointer(window));
    if (!input) return;

    // xoffset = horizontal wheel, yoffset = vertical wheel
    input->scrollDeltaY += (float)yoffset;
}

void Lexvi::Input::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    Lexvi::Input* input = reinterpret_cast<Lexvi::Input*>(glfwGetWindowUserPointer(window));
    if (!input) return;

    input->lastmousePos = input->mousePos;

    input->mousePos.x = static_cast<float>(xposIn);
    input->mousePos.y = static_cast<float>(yposIn);

    if (input->firstMouse)
    {
        input->lastmousePos = input->mousePos;
        input->firstMouse = false;
    }
    input->mouseDelta = input->mousePos - input->lastmousePos;
}

void Lexvi::Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Lexvi::Input* input = reinterpret_cast<Lexvi::Input*>(glfwGetWindowUserPointer(window));
    if (!input) return;

    KeyState& state = input->keys[key]; // inserts if missing

    if (action == GLFW_PRESS) {
        state.isPressed = true;
    }
    else if (action == GLFW_RELEASE) {
        state.isPressed = false;
    }
    else if (action == GLFW_REPEAT) {
        state.isHeld = true;
    }
}

void Lexvi::Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Lexvi::Input* input = reinterpret_cast<Lexvi::Input*>(glfwGetWindowUserPointer(window));
    if (!input) return;

    MouseButtonState& state = input->mouseButtons[button]; // inserts if missing

    if (action == GLFW_PRESS) {
        state.isPressed = true;
    }
    else if (action == GLFW_RELEASE) {
        state.isPressed = false;
    }
}
