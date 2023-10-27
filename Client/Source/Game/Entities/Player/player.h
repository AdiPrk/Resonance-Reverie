#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"
#include <Source/Math/rect.h>
#include <Source/Game/Levels/gameLevel.h>

class Texture2D;

// BallObject holds the state of the Ball object inheriting
// relevant state data from GameObject. Contains some extra
// functionality specific to Breakout's ball object that
// were too specific for within GameObject alone.
class Player : public GameObject
{
public:   
    // constructor(s)
    Player(Texture2D _sprite);

    void SetupRigidBody() override;

    // Update player
    void Update(float dt) override;
    void Draw(SpriteRenderer& renderer) override;
    void SetUpdatedPosition() override;
    
    // Reset player
    void Respawn();

    const Rect Bounds() const { return m_BoundingRect; }

    bool isGrounded;

private:
    friend GameLevel;

    void RespawnSelf();

    bool m_initPosition;
    glm::vec2 m_SpawnPosition;
    Rect m_BoundingRect;
    bool m_Respawn;

    // Movement
    bool m_CanJump = true;
    bool m_JumpHeld = false;
    float m_CoyoteTime = 0.0f, m_MaxCoyotoTime = 0.1f;
    float m_JumpBuffer = 0.0f, m_MaxJumpBuffer = 0.15f;
    bool m_JumpRequested = false;

    // Rendering
    glm::vec2 m_RenderSize;
};