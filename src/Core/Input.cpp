#include "Core/Input.h"
#include <GLFW/glfw3.h>
#include <cstring>

namespace MiniEngine {

GLFWwindow* Input::s_window     = nullptr;
float       Input::s_mouseDx   = 0.0f;
float       Input::s_mouseDy   = 0.0f;
float       Input::s_scrollDy  = 0.0f;
bool        Input::s_keyStates[512];
bool        Input::s_prevKeyStates[512];

void Input::setWindow(GLFWwindow* window) {
    s_window = window;
    std::memset(s_keyStates, 0, sizeof(s_keyStates));
    std::memset(s_prevKeyStates, 0, sizeof(s_prevKeyStates));
}

bool Input::keyDown(int glfwKey) {
    if (!s_window || glfwKey < 0 || glfwKey >= 512) return false;
    return s_keyStates[glfwKey];
}

bool Input::keyPressed(int glfwKey) {
    if (!s_window || glfwKey < 0 || glfwKey >= 512) return false;
    return s_keyStates[glfwKey] && !s_prevKeyStates[glfwKey];
}

bool Input::keyReleased(int glfwKey) {
    if (!s_window || glfwKey < 0 || glfwKey >= 512) return false;
    return !s_keyStates[glfwKey] && s_prevKeyStates[glfwKey];
}

void Input::getMouseDelta(float& dx, float& dy) {
    dx = s_mouseDx;
    dy = s_mouseDy;
}

void Input::setMouseDelta(float dx, float dy) {
    s_mouseDx = dx;
    s_mouseDy = dy;
}

void Input::addScrollDelta(float dy) {
    s_scrollDy += dy;
}

float Input::scrollDelta() {
    return s_scrollDy;
}

void Input::updateFromWindow() {
    if (!s_window) return;
    std::memcpy(s_prevKeyStates, s_keyStates, sizeof(s_keyStates));
    for (int i = 0; i < 512; ++i)
        s_keyStates[i] = (glfwGetKey(s_window, i) == GLFW_PRESS);
}

void Input::endFrame() {
    s_mouseDx = s_mouseDy = s_scrollDy = 0.0f;
}

} // namespace MiniEngine
