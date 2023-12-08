#include <PCH/pch.h>

#include "greyEnemy.h"
#include <Engine/Graphics/Texture/texture.h>
#include <Game/Physics/physicsWorld.h>
#include <Engine/Inputs/inputManager.h>

// constructor
GreyEnemy::GreyEnemy(glm::vec2 pos, glm::vec2 size, Dog::Texture2D sprite, glm::vec4 color, glm::vec2 velocity)
    : GameObject(pos, size, sprite, color, velocity)
{
    float randAngle = Dog::RandomFloat() * 6.28f;
    glm::vec2 vel = { cosf(randAngle), sinf(randAngle) };
    vel *= 3.f;

    m_Velocity = vel;
}

void GreyEnemy::SetupRigidBody() {
    if (m_RigidBody != nullptr) return;

    glm::vec2 bodyCenter = m_Position + m_Size / 2.f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody; // or b2_staticBody, b2_kinematicBody, depending on your needs
    bodyDef.position = PhysicsUtils::PixelsToMeters(bodyCenter);
    m_RigidBody = physicsWorld.CreateBody(&bodyDef);
    m_RigidBody->SetFixedRotation(true);
    m_RigidBody->SetGravityScale(0);

    b2CircleShape circleShape;
    circleShape.m_radius = PhysicsUtils::PixelsToMeters(m_Size.x / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.0f;
    fixtureDef.restitution = 1.0f;
    fixtureDef.restitutionThreshold = 0.0f;

    fixtureDef.filter.categoryBits = F_ENEMY;
    //fixtureDef.filter.groupIndex = 1;
    fixtureDef.filter.maskBits = 0xFFFF ^ (F_ENEMY | F_LAVA); // Collides with everything except enemy and lava

    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    m_RigidBody->CreateFixture(&fixtureDef);

    m_RigidBody->SetLinearVelocity(b2Vec2(m_Velocity.x, m_Velocity.y));
}

void GreyEnemy::Update(float dt) {
}

void GreyEnemy::SetUpdatedTransform()
{
    if (!m_RigidBody) return;

    m_Position = PhysicsUtils::MetersToPixels(m_RigidBody->GetPosition()) - m_Size / 2.f;
    m_Rotation = m_RigidBody->GetAngle();
}