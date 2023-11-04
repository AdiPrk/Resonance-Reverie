#include <PCH/pch.h>
#include "inputManager.h"
#include <Source/Graphics/Window/window.h>

//storing the keys and variables
KeyStates InputManager::keyStates[static_cast<int>(Key::TOTALKEYS)];
MouseStates InputManager::mouseStates[static_cast<int>(Mouse::LAST)];
GamepadButtonStates InputManager::prevGamepadButtonStates[MAX_GAMEPADS][static_cast<int>(GamepadButton::LAST)];
GamepadButtonStates InputManager::gamepadButtonStates[MAX_GAMEPADS][static_cast<int>(GamepadButton::LAST)];
GamepadAxisStates InputManager::gamepadAxisStates[MAX_GAMEPADS][static_cast<int>(GamepadAxis::LAST)];
GLFWwindow* InputManager::m_Window = nullptr;

float InputManager::degree;
float InputManager::previousDegree;
GLFWcursor* InputManager::standardCursor;


//setting some callbacks
void InputManager::Init(GLFWwindow* window)
{
	m_Window = window;

	glfwSetKeyCallback(window, KeyPressCallback);
	glfwSetMouseButtonCallback(window, MousePressCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);

	standardCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	glfwSetCursor(window, standardCursor);
}

void InputManager::ResetKeyStates() {
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
void InputManager::Update()
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

bool InputManager::GetKeyDown(Key key)
{
	if (key <= Key::UNKNOWN || key >= Key::TOTALKEYS)
	{
		return false;
	}

	return keyStates[static_cast<int>(key)].keyDown;
}

bool InputManager::GetKeyTriggered(Key key)
{
	if (key <= Key::UNKNOWN || key >= Key::TOTALKEYS)
	{
		return false;
	}

	return keyStates[static_cast<int>(key)].keyTriggered;
}

bool InputManager::GetKeyReleased(Key key)
{
	if (key <= Key::UNKNOWN || key >= Key::TOTALKEYS)
	{
		return false;
	}

	return keyStates[static_cast<int>(key)].keyReleased;
}

bool InputManager::GetKeyUp(Key key)
{
	if (key <= Key::UNKNOWN || key >= Key::TOTALKEYS)
	{
		return false;
	}

	return !GetKeyDown(key);
}

// Mouse

bool InputManager::GetMouseDown(Mouse mouseButton)
{
	return mouseStates[static_cast<int>(mouseButton)].mouseDown;
}

bool InputManager::GetMouseTriggered(Mouse mouseButton)
{
	return mouseStates[static_cast<int>(mouseButton)].mouseTriggered;
}

bool InputManager::GetMouseReleased(Mouse mouseButton)
{
	return mouseStates[static_cast<int>(mouseButton)].mouseReleased;
}

glm::vec2 InputManager::GetCursorPosition()
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

float InputManager::GetDegree()
{
	return degree;
}

float InputManager::GetPreviousDegree()
{
	return previousDegree;
}

void InputManager::SetPreviousDegree(float setDegree)
{
	previousDegree = setDegree;
}

// Callbacks

void InputManager::KeyPressCallback(GLFWwindow* windowPointer, int key, int scanCode, int action, int mod) noexcept
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

void InputManager::MousePressCallback(GLFWwindow* windowPointer, int mouseButton, int action, int mod) noexcept
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

void InputManager::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) noexcept
{
	degree -= static_cast<float>(yoffset);
	if (degree < -20.0f)
		degree = -20.0f;
	if (degree > 10.0f)
		degree = 10.0f;
}

// Gamepad

bool InputManager::GetGamepadPresent(int gamepad)
{
	return glfwJoystickIsGamepad(gamepad);
}

bool InputManager::GetGamepadButtonPressed(int gamepad, GamepadButton button)
{
	GLFWgamepadstate state;
	if (glfwGetGamepadState(gamepad, &state))
	{
		gamepadButtonStates[gamepad][static_cast<int>(button)].buttonDown = state.buttons[static_cast<int>(button)];

		return state.buttons[static_cast<int>(button)] == GLFW_PRESS;
	}
	return false;
}

bool InputManager::GetGamepadButtonTriggered(int gamepad, GamepadButton button)
{
	GLFWgamepadstate state;
	if (glfwGetGamepadState(gamepad, &state))
	{
		gamepadButtonStates[gamepad][static_cast<int>(button)].buttonDown = state.buttons[static_cast<int>(button)];
	}

	return !prevGamepadButtonStates[gamepad][static_cast<int>(button)].buttonDown &&
		gamepadButtonStates[gamepad][static_cast<int>(button)].buttonDown;
}

bool InputManager::GetGamepadButtonReleased(int gamepad, GamepadButton button)
{
	return prevGamepadButtonStates[gamepad][static_cast<int>(button)].buttonDown &&
		!gamepadButtonStates[gamepad][static_cast<int>(button)].buttonDown;
}

float InputManager::GetGamepadAxisValue(int gamepad, GamepadAxis axis)
{
	GLFWgamepadstate state;
	if (glfwGetGamepadState(gamepad, &state))
	{
		return state.axes[static_cast<int>(axis)];
	}
	return 0.0f;
}
