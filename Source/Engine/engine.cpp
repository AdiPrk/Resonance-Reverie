#include <PCH/pch.h>

#include "engine.h"
#include "application.h"
#include "ResourceManager/resourceManager.h"
#include <Engine/Graphics/Window/window.h>
#include <Engine/ResourceManager/resourceManager.h>
#include <Engine/Inputs/inputManager.h>
#include <Engine/Graphics/Renderer/renderer.h>

namespace Dog {

    DogEngine::DogEngine(unsigned int width, unsigned int height)
        : m_Application(nullptr)
    {
        m_Window = new Window(width, height);
        InputManager::Init(m_Window->GetWindow());
        Renderer::Init(width, height);
    }

    DogEngine::~DogEngine()
    {
        ResourceManager::Clear();
    }

    int DogEngine::Run(Application* app)
    {
        m_Application = app;

        /* Loop until the user closes the window */
        const float fixedTimeStep = 1.0f / 60.0f;
        float deltaTime = 0.0f;
        float lastTime = 0.0f;
        float accumulator = 0.0f;

        while (m_Window->IsRunning())
        {
            // Control FPS
            m_Window->LimitFPS();
            float currentTime = (float)glfwGetTime();
            deltaTime = std::min(currentTime - lastTime, 0.1f); // 10fps min
            accumulator += deltaTime;
            lastTime = currentTime;

            // Update game
            bool didFTS = false;
            while (accumulator >= fixedTimeStep)
            {
                glfwPollEvents();
                InputManager::Update();

                m_Application->Update(fixedTimeStep);

                accumulator -= fixedTimeStep;
                didFTS = true;
            }

            // Render!
            m_Application->Render(deltaTime, currentTime, accumulator / fixedTimeStep);

            // Get ready for next frame
            m_Window->SwapBuffers();
            if (didFTS) {
                InputManager::ResetKeyStates();
            }
        }

        ResourceManager::Clear();
        return EXIT_SUCCESS;
    }

}