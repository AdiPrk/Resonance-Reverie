#pragma once

#include <PCH/pch.h>
#include "Levels/gameLevel.h"
#include <Source/Graphics/Window/window.h>

// Represents the current state of the game
enum GameState {
    GAME_MENU = 0,
    GAME_ACTIVE,
    GAME_WIN
};

class Camera;
struct Rect;

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
struct OtherPlayer {
    glm::vec2 oldPos;
    glm::vec2 currPos;
    glm::vec2 renderPos;
};

class Game
{
public:
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    
    // initialize game state (load all shaders/textures/levels)
    void Init(Window* window);
    
    // game loop
    void ProcessInput(float dt);
    void SetPreviousPositions();
    void Update(float dt, float accumulator);
    void CalculateLerpedPositions(float t);
    void UpdateCamera(float dt);
    void Render(float dt, float currentTime, float t);
    void RenderColliders();

    // reset
    void ResetLevel();
    
    // utils
    void FilterRooms(float dt);
    const Rect Bounds() const { return m_RoomBounds; }
    void SetBounds(Rect bounds) { m_RoomBounds = bounds; }

#if DO_NETWORKING
    std::unordered_map<int, OtherPlayer> otherPlayers;
    void PushPlayer(int id);
    void PopPlayer(int id);
    void UpdatePlayer(int id, glm::vec2 pos);
#endif

    //std::vector<glm::vec2> otherPlayerPositions;
    glm::vec2 GetPlayerPosition();

private:
    friend GameLevel;
    
    Player* m_Player;
    Camera* m_Camera;
    unsigned int m_Width;
    unsigned int m_Height;
    Rect m_RoomBounds;
    unsigned int m_CurrentRoom;
    GameState m_State;
    std::vector<GameLevel> m_Rooms;
    Window* m_Window;
};