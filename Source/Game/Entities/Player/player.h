#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"
#include <Game/Levels/gameLevel.h>
#include "grapple.h"
#include <Game/game.h>

class Texture2D;
class Light;

enum PlayerStates {
    NORMAL = 0,
    GRAPPLE = 1,
    GRAPPLE_LAUNCH = 2
};

// BallObject holds the state of the Ball object inheriting
// relevant state data from GameObject. Contains some extra
// functionality specific to Breakout's ball object that
// were too specific for within GameObject alone.
class Player : public GameObject
{
public:   
    // constructor(s)
    Player(Game* game);

    void SetupRigidBody() override;

    // Update player
    void Update(float dt) override;
    void Draw(float dt) override;
    void SetUpdatedTransform() override;
    
    b2Body*& GetRigidbody() { return m_RigidBody; }

    // Reset player
    void Respawn();

    const Rect Bounds() const { return m_BoundingRect; }

    bool isGrounded;
    bool leftSensorGrounded = false;
    bool rightSensorGrounded = false;
    bool hitSolidObject = false;

    void AddGrapplePoint(const glm::vec2& point);
    void RemoveGrapplePoint(const glm::vec2& point);
    void CheckGrapple();

    const PlayerStates GetState() const { return m_State; }

private:
    friend GameLevel;

    void RespawnSelf();

    bool m_initPosition;
    glm::vec2 m_SpawnPosition;
    Rect m_BoundingRect;
    bool m_Respawn;
    bool m_Facing = false; // false is left

    // Movement
    bool m_CanJump = true;
    bool m_JumpHeld = false;
    float m_CoyoteTime = 0.0f, m_MaxCoyotoTime = 0.1f;
    float m_JumpBuffer = 0.0f, m_MaxJumpBuffer = 0.15f;
    bool m_JumpRequested = false;

    // Grapple
    SpiritGrapple m_Grapple;

    // Rendering
    glm::vec2 m_RenderSize;

    // Other
    PlayerStates m_State;

    std::vector<glm::vec2> m_GrapplePoints;
    glm::vec2 m_GrapplingTo;
    bool m_Grappling = false;
    float m_GrappleTime = 0.2f;
    float m_LaunchTime = 0.0f;
    Game* m_Game;
    Light* m_Light;
};