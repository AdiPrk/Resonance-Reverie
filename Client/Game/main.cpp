#include <PCH/pch.h>

#include <Engine/Graphics/Window/window.h>
#include <Engine/ResourceManager/resourceManager.h>
#include <Engine/Inputs/inputManager.h>

#include "game.h"

Game ResonanceReverie(SCREEN_WIDTH, SCREEN_HEIGHT);

int main() {
    // Create window and set callbacks
    Window window(SCREEN_WIDTH, SCREEN_HEIGHT);
    InputManager::Init(window.GetWindow());
    
    // Game init
    ResonanceReverie.Init(&window);

    /* Loop until the user closes the window */
    const float fixedTimeStep = 1.0f / 60.0f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float accumulator = 0.0f;

    while (window.IsRunning())
    {
        // Control FPS
        window.LimitFPS();
        float currentFrame = (float)glfwGetTime();
        deltaTime = std::min(currentFrame - lastFrame, 0.1f); // 10fps min
        accumulator += deltaTime;
        lastFrame = currentFrame;

        // Update game
        bool didFTS = false;
        while (accumulator >= fixedTimeStep)
        {
            glfwPollEvents();
            InputManager::Update();

            ResonanceReverie.SetPreviousPositions();
            ResonanceReverie.Update(fixedTimeStep, accumulator);

            accumulator -= fixedTimeStep;
            didFTS = true;
        }

        // Update camera
        ResonanceReverie.CalculateLerpedPositions(accumulator / fixedTimeStep);
        ResonanceReverie.UpdateCamera(deltaTime);

        // Render!
        ResonanceReverie.Render(deltaTime, currentFrame, accumulator / fixedTimeStep);

        // Get ready for next frame
        window.SwapBuffers();
        if (didFTS) {
            InputManager::ResetKeyStates();
        }
    }

    // delete all resources from the resource manager
    // ---------------------------------------------------------
    ResourceManager::Clear();
    
    return EXIT_SUCCESS;
}