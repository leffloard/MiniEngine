#pragma once

struct GLFWwindow;

namespace MiniEngine {

class Input {
public:
    static void setWindow(GLFWwindow* window);

    static bool keyDown(int glfwKey);
    static bool keyPressed(int glfwKey);
    static bool keyReleased(int glfwKey);

    static void getMouseDelta(float& dx, float& dy);
    static void setMouseDelta(float dx, float dy);
    static void addScrollDelta(float dy);
    static float scrollDelta();

    static void updateFromWindow();
    static void endFrame();

private:
    static GLFWwindow* s_window;
    static float s_mouseDx, s_mouseDy, s_scrollDy;
    static bool  s_keyStates[512];
    static bool  s_prevKeyStates[512];
};

} // namespace MiniEngine
