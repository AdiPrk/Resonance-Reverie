#include <PCH/pch.h>
#include "player.h"

#include <Game/Physics/physicsWorld.h>
#include <Engine/Inputs/inputManager.h>
#include <Engine/Graphics/Texture/texture.h>
#include <Engine/ResourceManager/resourceManager.h>
#include <Engine/Graphics/Renderer/Effects/postProcessor.h>
#include <Game/Entities/Environment/light.h>

// constructor(s)
Player::Player(Game* game)
    : GameObject(ResourceManager::GetTexture("ss2x3player"))
    , m_SpawnPosition(0)
    , m_BoundingRect(0, 0, 25.f, 25.f)
    , m_initPosition(true)
    , m_Respawn(false)
    , isGrounded(false)
    , m_State(PlayerStates::NORMAL)
    , m_GrapplingTo()
    , m_Game(game)
    , m_Light(nullptr)
{
    m_Size = { 32.f, 36.f };
    m_RenderSize = { 37.f, 42.f };

    m_Light = new Light(m_Position, 200.0f, 0.1f);
}

void Player::SetupRigidBody() {
    if (m_RigidBody != nullptr) return;

    glm::vec2 bodyCenter = m_Position + m_Size / 2.f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = PhysicsUtils::PixelsToMeters(bodyCenter);
    m_RigidBody = physicsWorld.CreateBody(&bodyDef);
    //m_RigidBody->SetFixedRotation(true);
    
    b2PolygonShape boxShape;
    boxShape.SetAsBox(PhysicsUtils::PixelsToMeters(m_Size.x / 2.0f),
                      PhysicsUtils::PixelsToMeters(m_Size.y / 2.0f));

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = 2.0f;
    fixtureDef.friction = 0.1f;
    fixtureDef.restitution = 0.0f;

    fixtureDef.filter.categoryBits = F_PLAYER;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    m_RigidBody->CreateFixture(&fixtureDef);

    m_RigidBody->SetAngularDamping(5.0f);

    // For foot sensors:
    float sensorHalfWidth = m_Size.x / 12.0f;
    float sensorHalfHeight = 3.0f;

    // Left Foot sensor fixture
    {
        b2PolygonShape footSensorShape;
        footSensorShape.SetAsBox(
            PhysicsUtils::PixelsToMeters(sensorHalfWidth),
            PhysicsUtils::PixelsToMeters(sensorHalfHeight),
            b2Vec2(
                PhysicsUtils::PixelsToMeters(-m_Size.x * 0.5f + sensorHalfWidth),
                PhysicsUtils::PixelsToMeters( m_Size.y * 0.5f + sensorHalfHeight)
            ),
            0.0f
        );

        b2FixtureDef footSensorFixtureDef;
        footSensorFixtureDef.shape = &footSensorShape;
        footSensorFixtureDef.isSensor = true;
        footSensorFixtureDef.filter.categoryBits = F_PLAYER_LEFTFOOT_SENSOR;
        footSensorFixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
        m_RigidBody->CreateFixture(&footSensorFixtureDef);
    }

    // Right Foot sensor fixture
    {
        b2PolygonShape footSensorShape;
        footSensorShape.SetAsBox(
            PhysicsUtils::PixelsToMeters(sensorHalfWidth),
            PhysicsUtils::PixelsToMeters(sensorHalfHeight),
            b2Vec2(
                PhysicsUtils::PixelsToMeters(m_Size.x * 0.5f - sensorHalfWidth),
                PhysicsUtils::PixelsToMeters(m_Size.y * 0.5f + sensorHalfHeight)
            ),
            0.0f
        );

        b2FixtureDef footSensorFixtureDef;
        footSensorFixtureDef.shape = &footSensorShape;
        footSensorFixtureDef.isSensor = true;
        footSensorFixtureDef.filter.categoryBits = F_PLAYER_RIGHTFOOT_SENSOR;
        footSensorFixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
        m_RigidBody->CreateFixture(&footSensorFixtureDef);
    }
}

void Player::CheckGrapple() {
    if (!InputManager::GetKeyTriggered(Key::C)) return;
    
    float minDist = FLT_MAX;
    bool canGrapple = false;
    glm::vec2 closestPoint;

    for (const auto& point : m_GrapplePoints) {
        float dist = glm::distance(point, m_Position);
        if (dist < minDist) {
            minDist = dist;
            closestPoint = point;
            canGrapple = true;
        }
    }

    if (canGrapple) {
        m_State = PlayerStates::GRAPPLE;
        m_GrapplingTo = closestPoint;
    }
}

// Update player
void Player::Update(float dt) {
    if (m_Respawn) {
        RespawnSelf();
        m_Respawn = false;
    }

    // Self righting
    if (!isGrounded || (!leftSensorGrounded || !rightSensorGrounded))
    {
        float angle = m_RigidBody->GetAngle();

        // Normalize the angle to a range of -PI to PI
        angle = angle - 2 * b2_pi * floor((angle + b2_pi) / (2 * b2_pi));

        // go the shortest way back to upright
        if (angle > b2_pi) {
            angle -= 2 * b2_pi;
        }
        else if (angle < -b2_pi) {
            angle += 2 * b2_pi;
        }

        float correctionSpeed = 6.0f;
        if (leftSensorGrounded ^ rightSensorGrounded) {

            if (isGrounded) {
                correctionSpeed *= 4.0f;
            }
        }

        float desiredAngularVelocity = -angle * correctionSpeed;
        desiredAngularVelocity = std::clamp(desiredAngularVelocity, -5.0f, 5.0f);

        // Apply the angular velocity to the rigid body
        m_RigidBody->SetAngularVelocity(desiredAngularVelocity);
    }

    switch (m_State) {
    case NORMAL: {
        // check grapple to switch states
        CheckGrapple();

        b2Vec2 vel = m_RigidBody->GetLinearVelocity();

        // printf("%i, %i\n", leftSensorGrounded, rightSensorGrounded);

        // Horizontal movement
        float horizontalSpeed = 1.6f;
        float dampingFactor = 0.74f;

        if (InputManager::GetKeyDown(Key::RIGHT)) {
            vel.x += horizontalSpeed;
        }
        if (InputManager::GetKeyDown(Key::LEFT)) {
            vel.x -= horizontalSpeed;
        }

        // Apply damping to horizontal velocity to simulate friction
        vel.x *= dampingFactor;

        // The Jump:
        // Update Coyote Time
        if (isGrounded) {
            m_CoyoteTime = m_MaxCoyotoTime; // Reset Coyote Time when on the ground
        }
        else {
            m_CoyoteTime -= dt; // Decrease Coyote Time when in the air
        }

        // Handle Jump Input
        if (InputManager::GetKeyDown(Key::C) && !m_JumpHeld) {
            m_JumpRequested = true;
            m_JumpBuffer = m_MaxJumpBuffer;
            m_JumpHeld = true;
        }

        // Handle Jump Execution
        bool jumpedUsingBuffer = false;
        if (m_JumpRequested && (isGrounded || m_CoyoteTime > 0)) {
            float jumpForce = -10.f;
            vel.y = jumpForce;
            m_JumpRequested = false; // Reset jump request
            jumpedUsingBuffer = true;

            ResourceManager::PlaySound("jump.ogg");
        }

        // Update Jump Buffer
        if (m_JumpBuffer > 0) {
            m_JumpBuffer -= dt; // Decrease Jump Buffer time
        }
        else {
            m_JumpRequested = false; // Clear jump request if jump buffer time expires
        }

        // Reset jump when the jump key is released
        if (InputManager::GetKeyUp(Key::C)) {
            if ((m_JumpHeld || jumpedUsingBuffer) && vel.y < 0) {
                vel.y *= 0.5f;
            }

            m_JumpHeld = false;
        }

        // Set the updated velocity to the rigid body
        m_RigidBody->SetLinearVelocity(vel);

        break;
    }
    case GRAPPLE: {

        GrapplingStatus grapStatus = m_Grapple.TryStartGrapple(m_RigidBody, m_Position + m_Size * 0.5f, m_GrapplingTo);
        m_JumpRequested = false;

        float slowMoTime = 0.35f;
        if (grapStatus == GrapplingStatus::STARTED) {
            m_Grappling = true;
            m_GrappleTime = 0.0f;
            m_Game->SetSlowMoTime(slowMoTime);
            m_Game->SetTimeFactor(0.2f);
        }

        m_GrappleTime += dt;

        m_Grapple.UpdateGrapple(dt);

        b2Vec2 vel = m_RigidBody->GetLinearVelocity();

        // Horizontal movement
        float horizontalSpeed = 0.15f;
        float dampingFactor = 0.99f;

        if (InputManager::GetKeyDown(Key::RIGHT)) {
            vel.x += horizontalSpeed;
        }
        if (InputManager::GetKeyDown(Key::LEFT)) {
            vel.x -= horizontalSpeed;
        }

        vel.x *= dampingFactor;

        m_RigidBody->SetLinearVelocity(vel);

        if (m_Game->GameIsSlowMo()) {
            //m_Game->Effects->shakeTime = 0.1f;
        }

        if (InputManager::GetKeyUp(Key::C) && m_Game->GameIsSlowMo()) {
            glm::vec2 v = m_GrapplingTo - (m_Position + m_Size / 2.0f);
            v = glm::normalize(v);
            v *= 50.0f;

            vel = b2Vec2(v.x, v.y);

            m_RigidBody->SetLinearVelocity({ 0.f, 0.f });
            m_RigidBody->ApplyLinearImpulseToCenter(vel, true);
            //m_RigidBody->SetLinearVelocity(vel);
            
            m_Grapple.ReleaseGrapple();
            m_Game->SetSlowMoTime(0.0f);

            m_State = GRAPPLE_LAUNCH;
            ResourceManager::PlaySound("jumpWoosh.mp3");
            m_Grappling = false;

            m_Game->Effects->shakeTime = 0.05f;

            break;
        }

        if (m_Grapple.GetLength() < 0.08f || InputManager::GetKeyUp(Key::C))
        {
            m_State = PlayerStates::NORMAL;
            m_Grapple.ReleaseGrapple();
            m_Grappling = false;
        }

        break;
    }
    case GRAPPLE_LAUNCH: {

        m_RigidBody->SetGravityScale(0.0f);
        m_RigidBody->SetLinearDamping(12.0f);
        m_LaunchTime += dt;

        if (m_LaunchTime > 0.2f || hitSolidObject)
        {
            m_State = PlayerStates::NORMAL;
            m_RigidBody->SetGravityScale(1.0f);
            m_RigidBody->SetLinearDamping(0.0f);
            m_LaunchTime = 0.0f;
        }

        break;
    }
    }

    hitSolidObject = false;
}

// Reset player
void Player::Respawn() {
    m_Respawn = true;
}

void Player::AddGrapplePoint(const glm::vec2& point)
{
    m_GrapplePoints.push_back(point);
}

void Player::RemoveGrapplePoint(const glm::vec2& point)
{
    auto it = std::find(m_GrapplePoints.begin(), m_GrapplePoints.end(), point);

    if (it != m_GrapplePoints.end())
    {
        m_GrapplePoints.erase(it);
    }
}

void Player::RespawnSelf()
{
    m_Position = m_SpawnPosition;

    m_RigidBody->SetTransform(PhysicsUtils::PixelsToMeters(m_SpawnPosition), 0.f);
    SetUpdatedTransform();
}

// Update transform from Box2D
void Player::SetUpdatedTransform()
{
    m_Position = PhysicsUtils::MetersToPixels(m_RigidBody->GetPosition()) - m_Size / 2.f;
    m_Rotation = m_RigidBody->GetAngle();

    m_BoundingRect.left = m_Position.x;
    m_BoundingRect.top = m_Position.y;
    m_BoundingRect.SetScale(m_Size);
}

void Player::Draw(SpriteRenderer& renderer, TextRenderer& textRenderer, float dt) {
    renderer.SetShader("sprite");

    glm::vec2 truerenderpos = m_RenderPosition - 1.0f;
    
    m_Animator.SetFirstIndex(0);
    m_Animator.SetLastIndex(1);
    m_Animator.SetFrameDuration(0.2f);

    //m_Animator.Update(dt);

    renderer.DrawSpriteFrame(m_Sprite, m_Animator.GetCurrentFrameIndex(), truerenderpos, m_RenderSize, m_Rotation);

    static float grappleLerp = 0.0f;
    if (m_Grappling) {
        grappleLerp += dt * 1.5f;
        grappleLerp = std::min(grappleLerp, 1.0f);
        // Calculating line info
        glm::vec2 playerCenter = m_RenderPosition + m_Size / 2.0f;
        glm::vec2 playerToAnchor = Lerp(playerCenter, m_GrapplingTo, grappleLerp);
        glm::vec2 anchorToPlayer = Lerp(m_GrapplingTo, playerCenter, grappleLerp);

        renderer.SetShader("saber");
        renderer.DrawLine(playerCenter, playerToAnchor, 6.0f, "square");
        renderer.DrawLine(anchorToPlayer, m_GrapplingTo, 6.0f, "square");
    }
    else {
        grappleLerp = 0;
    }

    m_Light->m_Position = m_RenderPosition + m_Size * 0.5f;
    Shader& pfx = ResourceManager::GetShader("sprite");
    std::string liStr = "lights[" + std::to_string(Light::lightIndex) + "]";
    pfx.SetVector2f(liStr + ".position", m_Light->m_Position);
    pfx.SetFloat(liStr + ".radius", m_Light->m_Radius);
    pfx.SetFloat(liStr + ".intensity", m_Light->m_Intensity);
    Light::lightIndex++;
}