#pragma once

#include <PCH/pch.h>

namespace Dog {

	class Window {
	public:
		Window(unsigned int screenWidth, unsigned int screenHeight);
		~Window();

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

		static unsigned int GetWidth() { return m_Width; }
		static unsigned int GetHeight() { return m_Height; }
		static float GetAspectRatio() { return m_AspectRatio; }

	private:
		std::chrono::high_resolution_clock::time_point m_LastFrameTime;
		static unsigned int m_Width;
		static unsigned int m_Height;
		static float m_AspectRatio;
		static GLFWwindow* m_Window;
	};

}