#pragma once

#include <PCH/pch.h>

#include <Source/Game/Entities/gameObject.h>

#include <Source/Math/rect.h>

class Player;
class SpriteRenderer;
class Game;

enum RoomCode {
    ROOM_NOT_FOUND = 0,
    ROOM_EXISTS,
    ROOM_CREATED
};

/// GameLevel holds all Tiles as part of a Breakout level and 
/// hosts functionality to Load/render levels from the harddisk.
class GameLevel
{
public:
    // level state
    std::vector<GameObject*> Entities;

    // constructor
    GameLevel() : m_ID(-1), m_BoundingRect(0, 0, 0, 0) { }
    ~GameLevel();

    // loads the starting level from file
    void LoadStarting(const char* filename, Game* game);

    // Loads the level the player is in, true if next room found
    RoomCode LoadNext(const char* filename, Game* game, const Rect& boundsToSearch, bool setAsCurrent = false);

    // render level
    void Draw(SpriteRenderer& renderer, float dt);

    const int ID() const { return m_ID; };
    const Rect Bounds() const { return m_BoundingRect; }
    float& DeleteTimer() { return m_OutOfFrameFor; }

    void SpawnEnemies(int enemyID, int numEnemies, Rect spawnBounds);

private:

    void SetupRoom(Game* game, auto element, bool starting, bool isCurrent, bool setAsCurrent);
    void SetCurrentRoomInfo(Game* game, Rect& roomBounds, float camScale, auto element);

    int m_ID;
    Rect m_BoundingRect;
    float m_OutOfFrameFor = 0;
};