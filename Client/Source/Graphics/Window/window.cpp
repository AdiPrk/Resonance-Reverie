#include <PCH/pch.h>
#include "window.h"
#include "Source/Graphics/Shader/shader.h"

void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

Window::Window(unsigned int screenWidth, unsigned int screenHeight) 
    : m_Window()
    , m_Width(screenWidth)
    , m_Height(screenHeight)
{
    m_LastFrameTime = std::chrono::high_resolution_clock::now();

    glfwWindowHint(GL_MAJOR_VERSION, 4);
    glfwWindowHint(GL_MINOR_VERSION, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

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
}

Window::~Window()
{
    if (m_Window) {
        glfwDestroyWindow(m_Window);
    }

    glfwTerminate();
}

void Window::BeginFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
}

void Window::EndFrame()
{
    /* Swap front and back buffers */
    glfwSwapBuffers(m_Window);
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