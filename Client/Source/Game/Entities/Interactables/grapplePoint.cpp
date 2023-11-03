#include <PCH/pch.h>

#include "grapplePoint.h"
#include <Source/Graphics/Texture/texture.h>
#include <Source/Game/Physics/physicsWorld.h>
#include <Source/Graphics/ResourceManager/resourceManager.h>

// constructor
GrapplePoint::GrapplePoint(glm::vec2 pos, glm::vec2 size, float radius, Texture2D sprite)
    : GameObject(pos, size, 0.0f, sprite, COLOR_F_BLOCK)
    , m_Radius(radius)
{
}

void GrapplePoint::SetupRigidBody() {
    if (m_RigidBody != nullptr) return;

    glm::vec2 bodyCenter = m_Position + m_Size / 2.f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position = PhysicsUtils::PixelsToMeters(bodyCenter);
    bodyDef.angle = m_Rotation;
    m_RigidBody = physicsWorld.CreateBody(&bodyDef);

    b2CircleShape circleShape;
    circleShape.m_radius = PhysicsUtils::PixelsToMeters(m_Radius);

    // Define the fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.isSensor = true;
    fixtureDef.filter.categoryBits = F_GRAPPLE_POINT;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

    m_RigidBody->CreateFixture(&fixtureDef);
}

void GrapplePoint::Draw(SpriteRenderer& renderer)
{
    renderer.SetShader(ResourceManager::GetShader("sprite"));
    renderer.DrawSprite(m_Sprite, m_RenderPosition, m_Size, m_Rotation, m_Color);

    glm::vec2 radSize(m_Radius);

    renderer.DrawSprite(m_Sprite, m_RenderPosition - radSize + m_Size * 0.5f, radSize * 2.0f, m_Rotation, { 1.0f, 1.0f, 1.0f, 0.1f });
}