#pragma once

#include <PCH/pch.h>
#include "Levels/gameLevel.h"
#include <Engine/Graphics/Window/window.h>
#include <Engine/application.h>
#include <Engine/Graphics/Renderer/Camera/camera.h>
#include <Engine/Graphics/Particles/particleEmitter.h>

class PostProcessor;

// Represents the current state of the game
enum GameState {
    GAME_MENU = 0,
    GAME_ACTIVE,
    GAME_WIN
};

struct Dog::Rect;

// Game holds all game-related state and functionality.
// Combines all game-related data
class Game : public Dog::Application
{
public:
    // constructor/destructor
    Game(Dog::Window* window);
    ~Game();
        
    // game loop
    void SetPreviousPositions();
    void Update(float dt) override;
    void CalculateLerpedPositions(float t);
    float CalculateSlowedDT(float dt);
    void UpdateCamera(float dt);
    void Render(float dt, float ct, float itf) override;
    void DrawScene(float dt);
    
    // utils
    void FilterRooms(float dt);
    const Dog::Rect& Bounds() const { return m_RoomBounds; }
    void SetBounds(const Dog::Rect& bounds) { m_RoomBounds = bounds; }
    void SetSlowMoTime(float t) { m_SlowMoTime = t; }
    void SetTimeFactor(float t) { m_SlowdownFactor = t; }
    bool GameIsSlowMo() const { return m_SlowMoTime > 0.0f; }

    glm::vec2 GetPlayerPosition();
    float GetPlayerRotation();    

private:
    friend GameLevel;
    
    Player* m_Player;
    Dog::Camera* m_Camera;
    unsigned int m_Width;
    unsigned int m_Height;
    Dog::Rect m_RoomBounds;
    GameState m_State;
    std::vector<GameLevel> m_Rooms;
    Dog::Window* m_Window;
    float m_SlowMoTime = 0;
    float m_SlowdownFactor = 1.0f;
    float m_FreezeTime = 0.0f;
    Dog::ParticleEmitter* backgroundEmitter;

    bool m_DrawColliders;
};