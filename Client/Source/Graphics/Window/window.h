#pragma once

#include <PCH/pch.h>

constexpr unsigned int SCREEN_WIDTH = 1600;
constexpr unsigned int SCREEN_HEIGHT = 900;
constexpr float WINDOW_ASPECT = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);

class Window {
public:
	Window(unsigned int screenWidth, unsigned int screenHeight);
	~Window();

	void BeginFrame();
	void EndFrame();
	int IsRunning();
	void SwapBuffers();
	void LimitFPS();
	void ToggleFullscreen();

	GLFWwindow* GetWindow() { return m_Window; };

	void SetKeyCallback(GLFWkeyfun callback) {
		glfwSetKeyCallback(m_Window, callback);
	}

	void SetFramebufferSizeCallback(GLFWframebuffersizefun callback) {
		glfwSetFramebufferSizeCallback(m_Window, callback);
	}

	void SetTitle(std::string& title) {
		glfwSetWindowTitle(m_Window, title.c_str());
	}

	constexpr static double targetFrameLength = 1.0 / 120.0;

private:
	std::chrono::high_resolution_clock::time_point m_LastFrameTime;
	unsigned int m_Width;
	unsigned int m_Height;
	GLFWwindow* m_Window;
};