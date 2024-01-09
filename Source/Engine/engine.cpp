#include <Engine/PCH/pch.h>

#include "engine.h"
#include "application.h"
#include "ResourceManager/resourceManager.h"
#include <Engine/Graphics/Window/window.h>
#include <Engine/ResourceManager/resourceManager.h>
#include <Engine/Inputs/inputManager.h>
#include <Engine/Graphics/Renderer/renderer.h>
#include "Scene/sceneManager.h"

namespace Dog {

    Window* DogEngine::m_Window = nullptr;
    Window* GetWindow() { return DogEngine::GetWindow(); } // Globally Available

    DogEngine::DogEngine(unsigned int width, unsigned int height)
    {
        m_Window = new Window(width, height);
        Input::Init(m_Window->GetWindow());
        Renderer::Init(width, height);
        RandomInit();
    }

    DogEngine::~DogEngine()
    {
        ResourceManager::Clear();
    }

    int DogEngine::Run(Scene* startScene)
    {
        SceneManager::Init(startScene);

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
                Input::Update();

                SceneManager::Update(fixedTimeStep);

                accumulator -= fixedTimeStep;
                didFTS = true;
            }

            // Render!
            Renderer::Update(deltaTime);
            SceneManager::Render(deltaTime, currentTime, accumulator / fixedTimeStep);

            // Get ready for next frame
            m_Window->SwapBuffers();
            if (didFTS) {
                Input::ResetKeyStates();
            }
        }

        ResourceManager::Clear();
        return EXIT_SUCCESS;
    }

}