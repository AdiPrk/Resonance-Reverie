#pragma once

#include <PCH/pch.h>

namespace Dog {

    struct KeyStates
    {
        bool keyDown = false;
        bool keyTriggered = false;
        bool keyReleased = false;
    };

    struct MouseStates
    {
        bool mouseDown = false;
        bool mouseTriggered = false;
        bool mouseReleased = false;
    };

    struct GamepadButtonStates
    {
        bool buttonDown = false;
    };

    struct GamepadAxisStates
    {
        float axisValue = 0.f;
    };

    enum class Key
    {
        // Unknown Key
        UNKNOWN = GLFW_KEY_UNKNOWN,

        // Printable Keys
        SPACE = GLFW_KEY_SPACE,
        APOSTROPHE = GLFW_KEY_APOSTROPHE,
        COMMA = GLFW_KEY_COMMA,
        MINUS = GLFW_KEY_MINUS,
        PERIOD = GLFW_KEY_PERIOD,
        SLASH = GLFW_KEY_SLASH,
        NUM0 = GLFW_KEY_0,
        NUM1 = GLFW_KEY_1,
        NUM2 = GLFW_KEY_2,
        NUM3 = GLFW_KEY_3,
        NUM4 = GLFW_KEY_4,
        NUM5 = GLFW_KEY_5,
        NUM6 = GLFW_KEY_6,
        NUM7 = GLFW_KEY_7,
        NUM8 = GLFW_KEY_8,
        NUM9 = GLFW_KEY_9,
        SEMICOLON = GLFW_KEY_SEMICOLON,
        EQUAL = GLFW_KEY_EQUAL,
        A = GLFW_KEY_A,
        B = GLFW_KEY_B,
        C = GLFW_KEY_C,
        D = GLFW_KEY_D,
        E = GLFW_KEY_E,
        F = GLFW_KEY_F,
        G = GLFW_KEY_G,
        H = GLFW_KEY_H,
        I = GLFW_KEY_I,
        J = GLFW_KEY_J,
        K = GLFW_KEY_K,
        L = GLFW_KEY_L,
        M = GLFW_KEY_M,
        N = GLFW_KEY_N,
        O = GLFW_KEY_O,
        P = GLFW_KEY_P,
        Q = GLFW_KEY_Q,
        R = GLFW_KEY_R,
        S = GLFW_KEY_S,
        T = GLFW_KEY_T,
        U = GLFW_KEY_U,
        V = GLFW_KEY_V,
        W = GLFW_KEY_W,
        X = GLFW_KEY_X,
        Y = GLFW_KEY_Y,
        Z = GLFW_KEY_Z,
        LEFTBRACKET = GLFW_KEY_LEFT_BRACKET,
        BACKSLASH = GLFW_KEY_BACKSLASH,
        RIGHTBRACKET = GLFW_KEY_RIGHT_BRACKET,
        GRAVEACCENT = GLFW_KEY_GRAVE_ACCENT,

        // Function Keys
        ESCAPE = GLFW_KEY_ESCAPE,
        ENTER = GLFW_KEY_ENTER,
        TAB = GLFW_KEY_TAB,
        BACKSPACE = GLFW_KEY_BACKSPACE,
        INSERT = GLFW_KEY_INSERT,
        //DELETE = GLFW_KEY_DELETE,
        RIGHT = GLFW_KEY_RIGHT,
        LEFT = GLFW_KEY_LEFT,
        DOWN = GLFW_KEY_DOWN,
        UP = GLFW_KEY_UP,
        PAGEUP = GLFW_KEY_PAGE_UP,
        PAGEDOWN = GLFW_KEY_PAGE_DOWN,
        HOME = GLFW_KEY_HOME,
        END = GLFW_KEY_END,
        CAPSLOCK = GLFW_KEY_CAPS_LOCK,
        SCROLLLOCK = GLFW_KEY_SCROLL_LOCK,
        NUMLOCK = GLFW_KEY_NUM_LOCK,
        PRINTSCREEN = GLFW_KEY_PRINT_SCREEN,
        BREAK = GLFW_KEY_PAUSE,
        F1 = GLFW_KEY_F1,
        F2 = GLFW_KEY_F2,
        F3 = GLFW_KEY_F3,
        F4 = GLFW_KEY_F4,
        F5 = GLFW_KEY_F5,
        F6 = GLFW_KEY_F6,
        F7 = GLFW_KEY_F7,
        F8 = GLFW_KEY_F8,
        F9 = GLFW_KEY_F9,
        F10 = GLFW_KEY_F10,
        F11 = GLFW_KEY_F11,
        F12 = GLFW_KEY_F12,
        KEYPAD0 = GLFW_KEY_KP_0,
        KEYPAD1 = GLFW_KEY_KP_1,
        KEYPAD2 = GLFW_KEY_KP_2,
        KEYPAD3 = GLFW_KEY_KP_3,
        KEYPAD4 = GLFW_KEY_KP_4,
        KEYPAD5 = GLFW_KEY_KP_5,
        KEYPAD6 = GLFW_KEY_KP_6,
        KEYPAD7 = GLFW_KEY_KP_7,
        KEYPAD8 = GLFW_KEY_KP_8,
        KEYPAD9 = GLFW_KEY_KP_9,
        KEYPADDECIMAL = GLFW_KEY_KP_DECIMAL,
        KEYPADMULTIPLY = GLFW_KEY_KP_MULTIPLY,
        KEYPADDIVIDE = GLFW_KEY_KP_DIVIDE,
        KEYPADMINUS = GLFW_KEY_KP_SUBTRACT,
        KEYPADPLUS = GLFW_KEY_KP_ADD,
        KEYPADENTER = GLFW_KEY_KP_ENTER,
        KEYPADEQUAL = GLFW_KEY_KP_EQUAL,
        LEFTSHIFT = GLFW_KEY_LEFT_SHIFT,
        LEFTCONTROL = GLFW_KEY_LEFT_CONTROL,
        LEFTALT = GLFW_KEY_LEFT_ALT,
        LEFTSUPER = GLFW_KEY_LEFT_SUPER,
        RIGHTSHIFT = GLFW_KEY_RIGHT_SHIFT,
        RIGHTCONTROL = GLFW_KEY_RIGHT_CONTROL,
        RIGHTALT = GLFW_KEY_RIGHT_ALT,
        RIGHTSUPER = GLFW_KEY_RIGHT_SUPER,
        MENU = GLFW_KEY_MENU,

        TOTALKEYS = GLFW_KEY_LAST
    };

    enum class Mouse
    {
        LEFT = GLFW_MOUSE_BUTTON_LEFT,
        RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
        MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
        BUTTON4 = GLFW_MOUSE_BUTTON_4,
        BUTTON5 = GLFW_MOUSE_BUTTON_5,
        BUTTON6 = GLFW_MOUSE_BUTTON_6,
        BUTTON7 = GLFW_MOUSE_BUTTON_7,
        BUTTON8 = GLFW_MOUSE_BUTTON_8,
        LAST = GLFW_MOUSE_BUTTON_LAST
    };

    enum class GamepadButton
    {
        A = GLFW_GAMEPAD_BUTTON_A,
        B = GLFW_GAMEPAD_BUTTON_B,
        X = GLFW_GAMEPAD_BUTTON_X,
        Y = GLFW_GAMEPAD_BUTTON_Y,
        LEFT_BUMPER = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
        RIGHT_BUMPER = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
        BACK = GLFW_GAMEPAD_BUTTON_BACK,
        START = GLFW_GAMEPAD_BUTTON_START,
        GUIDE = GLFW_GAMEPAD_BUTTON_GUIDE,
        LEFT_THUMB = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
        RIGHT_THUMB = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
        DPAD_UP = GLFW_GAMEPAD_BUTTON_DPAD_UP,
        DPAD_RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
        DPAD_DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
        DPAD_LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
        LAST = GLFW_GAMEPAD_BUTTON_LAST
    };

    enum class GamepadAxis
    {
        LEFT_X = GLFW_GAMEPAD_AXIS_LEFT_X,
        LEFT_Y = GLFW_GAMEPAD_AXIS_LEFT_Y,
        RIGHT_X = GLFW_GAMEPAD_AXIS_RIGHT_X,
        RIGHT_Y = GLFW_GAMEPAD_AXIS_RIGHT_Y,
        LEFT_TRIGGER = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
        RIGHT_TRIGGER = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
        LAST = GLFW_GAMEPAD_AXIS_LAST
    };

}