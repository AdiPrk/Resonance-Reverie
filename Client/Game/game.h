#pragma once

#include <PCH/pch.h>
#include "Levels/gameLevel.h"
#include <Engine/Graphics/Window/window.h>

class PostProcessor;

// Represents the current state of the game
enum GameState {
    GAME_MENU = 0,
    GAME_ACTIVE,
    GAME_WIN
};

class Camera;
struct Rect;
class ParticleEmitter;

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    
    // initialize game state (load all shaders/textures/levels)
    void Init(Window* window);
    
    // game loop
    void SetPreviousPositions();
    void Update(float dt, float accumulator);
    void CalculateLerpedPositions(float t);
    float CalculateSlowedDT(float dt);
    void UpdateCamera(float dt);
    void Render(float dt, float currentTime, float t);
    void DrawScene(float dt, float t);
    
    // utils
    void FilterRooms(float dt);
    const Rect& Bounds() const { return m_RoomBounds; }
    void SetBounds(const Rect& bounds) { m_RoomBounds = bounds; }
    void SetSlowMoTime(float t) { m_SlowMoTime = t; }
    void SetTimeFactor(float t) { m_SlowdownFactor = t; }
    bool GameIsSlowMo() const { return m_SlowMoTime > 0.0f; }

    glm::vec2 GetPlayerPosition();
    float GetPlayerRotation();

    PostProcessor* Effects;

    

private:
    friend GameLevel;
    
    Player* m_Player;
    Camera* m_Camera;
    unsigned int m_Width;
    unsigned int m_Height;
    Rect m_RoomBounds;
    GameState m_State;
    std::vector<GameLevel> m_Rooms;
    Window* m_Window;
    float m_SlowMoTime = 0;
    float m_SlowdownFactor = 1.0f;
    float m_FreezeTime = 0.0f;
    ParticleEmitter* backgroundEmitter;

    bool m_DrawColliders;
};