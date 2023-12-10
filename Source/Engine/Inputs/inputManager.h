#pragma once

#include <Engine/PCH/pch.h>
#include "glfwInputStates.h"

namespace Dog {

// GLFW supports 16 controllers
#define MAX_GAMEPADS 16

	class Input
	{
	public:
		// Init
		static void Init(GLFWwindow* window);

		// Update states
		static void ResetKeyStates();
		static void Update();

		// Keys
		static bool GetKeyDown(Key key);
		static bool GetKeyTriggered(Key key);
		static bool GetKeyReleased(Key key);
		static bool GetKeyUp(Key key);

		// Mouse
		static bool GetMouseDown(Mouse mouseButton);
		static bool GetMouseTriggered(Mouse mouseButton);
		static bool GetMouseReleased(Mouse mouseButton);

		static glm::vec2 GetCursorPosition();

		static float GetDegree();
		static float GetPreviousDegree();
		static void  SetPreviousDegree(float setDegree);

		// Gamepad
		static bool GetGamepadPresent(int gamepad);
		static bool GetGamepadButtonPressed(int gamepad, GamepadButton button);
		static bool GetGamepadButtonTriggered(int gamepad, GamepadButton button);
		static bool GetGamepadButtonReleased(int gamepad, GamepadButton button);
		static float GetGamepadAxisValue(int gamepad, GamepadAxis axis);

	private:
		// Callbacks
		static void KeyPressCallback(GLFWwindow* windowPointer, int key, int scanCode, int action, int mod) noexcept;
		static void MousePressCallback(GLFWwindow* windowPointer, int mouseButton, int action, int mod) noexcept;
		static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) noexcept;

		static GLFWcursor* standardCursor;

		// States
		static KeyStates keyStates[static_cast<int>(Key::TOTALKEYS)];
		static MouseStates mouseStates[static_cast<int>(Mouse::LAST)];
		static GamepadButtonStates prevGamepadButtonStates[MAX_GAMEPADS][static_cast<int>(GamepadButton::LAST)];
		static GamepadButtonStates gamepadButtonStates[MAX_GAMEPADS][static_cast<int>(GamepadButton::LAST)];
		static GamepadAxisStates gamepadAxisStates[MAX_GAMEPADS][static_cast<int>(GamepadAxis::LAST)];

		static float degree;
		static float previousDegree;

		static GLFWwindow* m_Window;
	};

}