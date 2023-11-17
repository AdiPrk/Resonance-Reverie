#include <PCH/pch.h>

#include "Graphics/Window/window.h"
#include "Graphics/ResourceManager/resourceManager.h"
#include "Game/game.h"
#include "Game/Inputs/inputManager.h"
#include "Networking/networking.h"

// function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

Game ResonanceReverie(SCREEN_WIDTH, SCREEN_HEIGHT);

int main() {
#if DO_NETWORKING
    ENetPeer* peer = createAndConnectClient();
    if (!peer) {
        printf("couldn't connect :(");
        return EXIT_FAILURE;
    }

    sendPacket(peer, MESSAGE_PACKET, "Hello, Server!");

    std::thread netThread(networkThread, peer, &ResonanceReverie);
#endif

    // Create window and set callbacks
    Window window(SCREEN_WIDTH, SCREEN_HEIGHT);
    InputManager::Init(window.GetWindow());
    //window.SetKeyCallback(InputManager::KeyCallback);
    window.SetFramebufferSizeCallback(framebuffer_size_callback);
    
    // Game init
    ResonanceReverie.Init(&window);

    /* Loop until the user closes the window */
    const float fixedTimeStep = 1.0f / 60.0f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float accumulator = 0.0f;

#if DO_NETWORKING
    glm::vec2 playerPos = ResonanceReverie.GetPlayerPosition();
    sendPacketVec2(peer, INIT_PLAYER_PACKET, playerPos.x, playerPos.y);
#endif

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

#if DO_NETWORKING
            glm::vec2 playerPos = ResonanceReverie.GetPlayerPosition();
            float playerRot = ResonanceReverie.GetPlayerRotation();
            sendPacketVec2(peer, POSITION_PACKET, playerPos.x, playerPos.y);
            sendPacketFloat(peer, ROTATION_PACKET, playerRot);
#endif

            accumulator -= fixedTimeStep;
            didFTS = true;
        }

        // Update camera
        ResonanceReverie.CalculateLerpedPositions(accumulator / fixedTimeStep);
        ResonanceReverie.UpdateCamera(deltaTime);

        // Render!
        ResonanceReverie.Render(deltaTime, currentFrame, accumulator / fixedTimeStep);
        window.SwapBuffers();

        // Get ready for next frame
        if (didFTS) {
            InputManager::ResetKeyStates();
        }
    }

    // delete all resources from the resource manager
    // ---------------------------------------------------------
    ResourceManager::Clear();

#if DO_NETWORKING
    enet_peer_disconnect(peer, 0);
    netThread.join();
#endif

    return EXIT_SUCCESS;
}

// Keep our things in the center of the screen with the correct aspect ratio
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    int newWidth = width;
    int newHeight = (int)(width / WINDOW_ASPECT);

    if (newHeight > height)
    {
        newHeight = height;
        newWidth = (int)(height * WINDOW_ASPECT);
    }

    int xOffset = (width - newWidth) / 2;
    int yOffset = (height - newHeight) / 2;

    glViewport(xOffset, yOffset, newWidth, newHeight);
}