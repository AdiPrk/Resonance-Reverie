#pragma once

#include <PCH/pch.h>

namespace InputManager {

    bool GetKeyDown(int key);
    bool GetKeyUp(int key);
    bool GetKeyDownOnce(int key);
    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

} // namespace