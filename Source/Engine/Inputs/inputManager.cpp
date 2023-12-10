#include <Engine/PCH/pch.h>
#include "inputManager.h"
#include <Engine/Graphics/Window/window.h>

namespace Dog {

	//storing the keys and variables
	KeyStates Input::keyStates[static_cast<int>(Key::TOTALKEYS)];
	MouseStates Input::mouseStates[static_cast<int>(Mouse::LAST)];
	GamepadButtonStates Input::prevGamepadButtonStates[MAX_GAMEPADS][static_cast<int>(GamepadButton::LAST)];
	GamepadButtonStates Input::gamepadButtonStates[MAX_GAMEPADS][static_cast<int>(GamepadButton::LAST)];
	GamepadAxisStates Input::gamepadAxisStates[MAX_GAMEPADS][static_cast<int>(GamepadAxis::LAST)];
	GLFWwindow* Input::m_Window = nullptr;

	float Input::degree;
	float Input::previousDegree;
	GLFWcursor* Input::standardCursor;


	//setting some callbacks
	void Input::Init(GLFWwindow* window)
	{
		m_Window = window;

		glfwSetKeyCallback(window, KeyPressCallback);
		glfwSetMouseButtonCallback(window, MousePressCallback);
		glfwSetScrollCallback(window, MouseScrollCallback);

		standardCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		glfwSetCursor(window, standardCursor);
	}

	void Input::ResetKeyStates() {
		for (auto& key : keyStates)
		{
			key.keyTriggered = false;
			key.keyReleased = false;
		}

		for (auto& mouse : mouseStates)
		{
			mouse.mouseTriggered = false;
			mouse.mouseReleased = false;
		}
	}

	// update
	void Input::Update()
	{
		if (GetKeyDown(Key::ESCAPE))
			glfwSetWindowShouldClose(m_Window, true);

		for (int i = 0; i < MAX_GAMEPADS; i++)
		{
			for (int j = 0; j <= static_cast<int>(GamepadButton::LAST); j++)
			{
				prevGamepadButtonStates[i][j].buttonDown = gamepadButtonStates[i][j].buttonDown;
			}
		}
	}

	// Keys

	bool Input::GetKeyDown(Key key)
	{
		if (key <= Key::UNKNOWN || key >= Key::TOTALKEYS)
		{
			return false;
		}

		return keyStates[static_cast<int>(key)].keyDown;
	}

	bool Input::GetKeyTriggered(Key key)
	{
		if (key <= Key::UNKNOWN || key >= Key::TOTALKEYS)
		{
			return false;
		}

		return keyStates[static_cast<int>(key)].keyTriggered;
	}

	bool Input::GetKeyReleased(Key key)
	{
		if (key <= Key::UNKNOWN || key >= Key::TOTALKEYS)
		{
			return false;
		}

		return keyStates[static_cast<int>(key)].keyReleased;
	}

	bool Input::GetKeyUp(Key key)
	{
		if (key <= Key::UNKNOWN || key >= Key::TOTALKEYS)
		{
			return false;
		}

		return !GetKeyDown(key);
	}

	// Mouse

	bool Input::GetMouseDown(Mouse mouseButton)
	{
		return mouseStates[static_cast<int>(mouseButton)].mouseDown;
	}

	bool Input::GetMouseTriggered(Mouse mouseButton)
	{
		return mouseStates[static_cast<int>(mouseButton)].mouseTriggered;
	}

	bool Input::GetMouseReleased(Mouse mouseButton)
	{
		return mouseStates[static_cast<int>(mouseButton)].mouseReleased;
	}

	glm::vec2 Input::GetCursorPosition()
	{
		/*GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		ViewportInfo viewportInfo(viewport);

		double cursorX, cursorY;
		glfwGetCursorPos(Window::GetWindowHandle(), &cursorX, &cursorY);
		cursorY = -cursorY;

		cursorX -= viewportInfo.x;
		cursorY += viewportInfo.y;

		double ratioX = cursorX / viewportInfo.width;
		double ratioY = cursorY / viewportInfo.height;
		cursorX = ratioX * SCREEN_WIDTH;
		cursorY = ratioY * SCREEN_HEIGHT;

		return glm::vec2((float)cursorX, (float)cursorY);*/
		return { 0.f, 0.f };
	}

	float Input::GetDegree()
	{
		return degree;
	}

	float Input::GetPreviousDegree()
	{
		return previousDegree;
	}

	void Input::SetPreviousDegree(float setDegree)
	{
		previousDegree = setDegree;
	}

	// Callbacks

	void Input::KeyPressCallback(GLFWwindow* windowPointer, int key, int scanCode, int action, int mod) noexcept
	{
		(void)windowPointer;
		(void)scanCode;
		(void)mod;

		if (action == GLFW_PRESS)
		{
			keyStates[key].keyTriggered = true;
			keyStates[key].keyDown = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keyStates[key].keyReleased = true;
			keyStates[key].keyDown = false;
		}
	}

	void Input::MousePressCallback(GLFWwindow* windowPointer, int mouseButton, int action, int mod) noexcept
	{
		(void)windowPointer;
		(void)mod;

		if (action == GLFW_PRESS)
		{
			mouseStates[mouseButton].mouseTriggered = true;
			mouseStates[mouseButton].mouseDown = true;
		}
		else if (action == GLFW_RELEASE)
		{
			mouseStates[mouseButton].mouseReleased = true;
			mouseStates[mouseButton].mouseDown = false;
		}
	}

	void Input::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) noexcept
	{
		degree -= static_cast<float>(yoffset);
		if (degree < -20.0f)
			degree = -20.0f;
		if (degree > 10.0f)
			degree = 10.0f;
	}

	// Gamepad

	bool Input::GetGamepadPresent(int gamepad)
	{
		return glfwJoystickIsGamepad(gamepad);
	}

	bool Input::GetGamepadButtonPressed(int gamepad, GamepadButton button)
	{
		GLFWgamepadstate state;
		if (glfwGetGamepadState(gamepad, &state))
		{
			gamepadButtonStates[gamepad][static_cast<int>(button)].buttonDown = state.buttons[static_cast<int>(button)];

			return state.buttons[static_cast<int>(button)] == GLFW_PRESS;
		}
		return false;
	}

	bool Input::GetGamepadButtonTriggered(int gamepad, GamepadButton button)
	{
		GLFWgamepadstate state;
		if (glfwGetGamepadState(gamepad, &state))
		{
			gamepadButtonStates[gamepad][static_cast<int>(button)].buttonDown = state.buttons[static_cast<int>(button)];
		}

		return !prevGamepadButtonStates[gamepad][static_cast<int>(button)].buttonDown &&
			gamepadButtonStates[gamepad][static_cast<int>(button)].buttonDown;
	}

	bool Input::GetGamepadButtonReleased(int gamepad, GamepadButton button)
	{
		return prevGamepadButtonStates[gamepad][static_cast<int>(button)].buttonDown &&
			!gamepadButtonStates[gamepad][static_cast<int>(button)].buttonDown;
	}

	float Input::GetGamepadAxisValue(int gamepad, GamepadAxis axis)
	{
		GLFWgamepadstate state;
		if (glfwGetGamepadState(gamepad, &state))
		{
			return state.axes[static_cast<int>(axis)];
		}
		return 0.0f;
	}


}