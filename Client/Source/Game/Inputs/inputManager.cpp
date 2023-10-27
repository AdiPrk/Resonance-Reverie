#include <PCH/pch.h>
#include "inputManager.h"

namespace InputManager {

    bool Keys[1024];
    bool KeysProcessed[1024];

    bool GetKeyDown(int key) {
        if (key >= 0 && key < 1024) {
            return Keys[key];
        }

        return false;
    }

    bool GetKeyUp(int key) {
        if (key >= 0 && key < 1024) {
            return !Keys[key];
        }

        return false;
    }

    // Only true one time after key is pressed
    bool GetKeyDownOnce(int key) {
        if (key >= 0 && key < 1024) {
            if (Keys[key] && !KeysProcessed[key]) {
                KeysProcessed[key] = true;
                return Keys[key];
            }
        }

        return false;
    }

    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (key >= 0 && key < 1024) {
            if (action == GLFW_PRESS)
                Keys[key] = true;
            else if (action == GLFW_RELEASE) {
                Keys[key] = false;
                KeysProcessed[key] = false;
            }
        }
    }

} // namespace