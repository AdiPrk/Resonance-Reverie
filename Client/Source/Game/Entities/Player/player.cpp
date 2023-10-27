#include <PCH/pch.h>
#include "player.h"

#include <Source/Game/Physics/physicsWorld.h>
#include <Source/Game/Inputs/inputManager.h>
#include <Source/Graphics/Texture/texture.h>
#include <Source/Graphics/ResourceManager/resourceManager.h>

// constructor(s)
Player::Player(Texture2D _sprite)
    : GameObject(_sprite, { 0.2f, 1.f, 0.4f, 1.f }) // sprite, color
    , m_SpawnPosition(0)
    , m_BoundingRect(0, 0, 25.f, 25.f)
    , m_initPosition(true)
    , m_Respawn(false)
    , isGrounded(false)
{
    m_Size = { 21.f, 38.f };
    m_RenderSize = { 23.f, 40.f };
}

void Player::SetupRigidBody() {
    if (m_RigidBody != nullptr) return;

    glm::vec2 bodyCenter = m_Position + m_Size / 2.f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody; // or b2_staticBody, b2_kinematicBody, depending on your needs
    bodyDef.position = PhysicsUtils::PixelsToMeters(bodyCenter);
    m_RigidBody = physicsWorld.CreateBody(&bodyDef);
    m_RigidBody->SetFixedRotation(true);
    
    b2PolygonShape boxShape;
    boxShape.SetAsBox(PhysicsUtils::PixelsToMeters(m_Size.x / 2.0f),
        PhysicsUtils::PixelsToMeters(m_Size.y / 2.0f));

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = 10.0f;
    fixtureDef.friction = 0.2f;
    fixtureDef.restitution = 0.0f;

    fixtureDef.filter.categoryBits = F_PLAYER;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    m_RigidBody->CreateFixture(&fixtureDef);
}

// Update player
void Player::Update(float dt) {
    if (m_Respawn) {
        RespawnSelf();
        m_Respawn = false;
    }

    b2Vec2 vel = m_RigidBody->GetLinearVelocity();

    float horizontalSpeed = 1.2f;
    if (InputManager::GetKeyDown(GLFW_KEY_Z)) {
        horizontalSpeed = 0.5f;
    }
    float dampingFactor = 0.84f;
    
    // Horizontal Movement: Right and Left
    if (InputManager::GetKeyDown(GLFW_KEY_RIGHT)) {
        vel.x += horizontalSpeed;
    }
    if (InputManager::GetKeyDown(GLFW_KEY_LEFT)) {
        vel.x -= horizontalSpeed;
    }

    // Apply damping to horizontal velocity to simulate friction
    vel.x *= dampingFactor;

    // The Jump
    // Update Coyote Time
    if (isGrounded) {
        m_CoyoteTime = m_MaxCoyotoTime; // Reset Coyote Time when on the ground
    }
    else {
        m_CoyoteTime -= dt; // Decrease Coyote Time when in the air
    }

    // Handle Jump Input
    if (InputManager::GetKeyDown(GLFW_KEY_C) && !m_JumpHeld) {
        m_JumpRequested = true;
        m_JumpBuffer = m_MaxJumpBuffer;
        m_JumpHeld = true;
    }

    // Handle Jump Execution
    if (m_JumpRequested && (isGrounded || m_CoyoteTime > 0)) {
        float jumpForce = -14.f;
        vel.y = jumpForce;
        m_JumpRequested = false; // Reset jump request
    }

    // Update Jump Buffer
    if (m_JumpBuffer > 0) {
        m_JumpBuffer -= dt; // Decrease Jump Buffer time
    }
    else {
        m_JumpRequested = false; // Clear jump request if jump buffer time expires
    }

    // Reset jump when the jump key is released
    if (InputManager::GetKeyUp(GLFW_KEY_C)) {
        if (m_JumpHeld && vel.y < 0) {
            vel.y *= 0.5f;
        }
         
        m_JumpHeld = false;
    }

    // Set the updated velocity to the rigid body
    m_RigidBody->SetLinearVelocity(vel);
}

// Reset player
void Player::Respawn() {
    m_Respawn = true;
}

void Player::RespawnSelf()
{
    m_Position = m_SpawnPosition;

    m_RigidBody->SetTransform(PhysicsUtils::PixelsToMeters(m_SpawnPosition), 0.f);
    SetUpdatedPosition();
}

// When you need to get positions or sizes from Box2D

void Player::SetUpdatedPosition()
{
    m_Position = PhysicsUtils::MetersToPixels(m_RigidBody->GetPosition()) - m_Size / 2.f;
    m_Rotation = m_RigidBody->GetAngle();

    m_BoundingRect.left = m_Position.x;
    m_BoundingRect.top = m_Position.y;
    m_BoundingRect.SetScale(m_Size);
}

void Player::Draw(SpriteRenderer& renderer) {
    renderer.SetShader(ResourceManager::GetShader("sprite"));
    renderer.DrawSprite(m_Sprite, m_RenderPosition - 1.0f, m_RenderSize, m_Rotation, m_Color);
}