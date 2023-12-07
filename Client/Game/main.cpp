#include <PCH/pch.h>

#include <Engine/Graphics/Window/window.h>
#include <Engine/ResourceManager/resourceManager.h>
#include <Engine/Inputs/inputManager.h>
#include <Engine/Networking/networking.h>

#include "game.h"

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

        // Get ready for next frame
        window.SwapBuffers();
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