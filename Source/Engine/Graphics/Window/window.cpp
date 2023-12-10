#include <Engine/PCH/pch.h>
#include "window.h"
#include "Engine/Graphics/Shader/shader.h"

namespace Dog {

    void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    void glfw_error_callback(int error, const char* description)
    {
        std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
    }

#define DEBUG_OPENGL_OUTPUT 1

#if DEBUG_OPENGL_OUTPUT
    void GLAPIENTRY OpenGLDebugCallback(GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam) {
        // Displaying the debug message
        if (type == GL_DEBUG_TYPE_OTHER) return;

        std::cerr << "OpenGL Debug Message (ID: " << id << "):\n";
        std::cerr << "Message: " << message << "\n";
        std::cerr << "Source: ";

        switch (source) {
        case GL_DEBUG_SOURCE_API: std::cerr << "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cerr << "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: std::cerr << "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: std::cerr << "Application"; break;
        case GL_DEBUG_SOURCE_OTHER: std::cerr << "Other"; break;
        }

        std::cerr << "\nType: ";

        switch (type) {
        case GL_DEBUG_TYPE_ERROR: std::cerr << "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Deprecated Behavior"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cerr << "Undefined Behavior"; break;
        case GL_DEBUG_TYPE_PORTABILITY: std::cerr << "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: std::cerr << "Performance"; break;
        case GL_DEBUG_TYPE_OTHER: std::cerr << "Other"; break;
        }

        std::cerr << "\nSeverity: ";

        switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: std::cerr << "High"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: std::cerr << "Medium"; break;
        case GL_DEBUG_SEVERITY_LOW: std::cerr << "Low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "Notification"; break;
        }

        std::cerr << "\n\n";

        if (type == GL_DEBUG_TYPE_ERROR) {
            __debugbreak();
        }
    }
#endif

    unsigned int Window::m_Width = 0;
    unsigned int Window::m_Height = 0;
    float Window::m_AspectRatio = 0;
    GLFWwindow* Window::m_Window = nullptr;

    Window::Window(unsigned int screenWidth, unsigned int screenHeight)
    {
        m_Width = screenWidth;
        m_Height = screenHeight;
        m_AspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);

        m_LastFrameTime = std::chrono::high_resolution_clock::now();

        glfwWindowHint(GL_MAJOR_VERSION, 4);
        glfwWindowHint(GL_MINOR_VERSION, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        //glDepthFunc(GL_LEQUAL);
        //glEnable(GL_STENCIL_TEST);

        if (!glfwInit()) {
            std::cout << "Oh Noes, no glfw init!";
        }

        glfwSetErrorCallback(glfw_error_callback);

        m_Window = glfwCreateWindow((int)screenWidth, (int)screenHeight, "Resonance Reverie", NULL, NULL);
        glViewport(0, 0, (int)screenWidth, (int)screenHeight);

        if (!m_Window)
        {
            const char* description;
            int error = glfwGetError(&description);
            if (error != GLFW_NO_ERROR)
            {
                fprintf(stderr, "Error Creating Window: %d: %s\n", error, description);
            }

            glfwTerminate();
            return;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(m_Window);

        if (glewInit() != GLEW_OK)
            std::cout << "oh noes, glew didn't init properly!" << std::endl;

        std::cout << glGetString(GL_VERSION) << std::endl;

        //glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glfwSwapInterval(0);

        SetFramebufferSizeCallback(framebuffer_size_callback);

#if DEBUG_OPENGL_OUTPUT
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLDebugCallback, nullptr);
        //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif
    }

    Window::~Window()
    {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
        }

        glfwTerminate();
    }

    int Window::IsRunning() {
        return glfwWindowShouldClose(m_Window) == 0;
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_Window);
    }

    void Window::LimitFPS()
    {
        // Get current time using high-resolution clock
        auto current_time = std::chrono::high_resolution_clock::now();

        // Calculate the frame duration in seconds
        double frameLength = std::chrono::duration<double>(current_time - m_LastFrameTime).count();

        if (frameLength < targetFrameLength/* - 0.001*/) // 0.001 threshold to avoid micro sleeps
        {
            // Calculate how long to sleep in order to maintain the target frame rate.
            auto sleep_duration = std::chrono::duration<double>(targetFrameLength - frameLength);
            std::this_thread::sleep_for(sleep_duration);

            // After sleeping, adjust the frame length.
            frameLength = targetFrameLength;
        }

        // Update lastFrameTime
        m_LastFrameTime = current_time;
    }

    int windowedWidth = 1600, windowedHeight = 900;
    int windowedPosX = 100, windowedPosY = 100;

    void Window::ToggleFullscreen()
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        if (glfwGetWindowMonitor(m_Window)) {
            // If in fullscreen mode, switch to windowed mode with previous window size and position
            glfwSetWindowMonitor(m_Window, nullptr, windowedPosX, windowedPosY, windowedWidth, windowedHeight, 0);
        }
        else {
            // Store the current window size and position before switching to fullscreen
            glfwGetWindowPos(m_Window, &windowedPosX, &windowedPosY);
            glfwGetWindowSize(m_Window, &windowedWidth, &windowedHeight);

            // Switch to fullscreen mode
            glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        int newWidth = width;
        int newHeight = (int)(width / Window::GetAspectRatio());

        if (newHeight > height)
        {
            newHeight = height;
            newWidth = (int)(height * Window::GetAspectRatio());
        }

        int xOffset = (width - newWidth) / 2;
        int yOffset = (height - newHeight) / 2;

        glViewport(xOffset, yOffset, newWidth, newHeight);
    }

}