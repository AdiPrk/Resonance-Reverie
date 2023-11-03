#include <PCH/pch.h>

#include "lava.h"
#include <Source/Graphics/Texture/texture.h>
#include <Source/Game/Physics/physicsWorld.h>
#include <Source/Graphics/ResourceManager/resourceManager.h>

// constructor
Lava::Lava(glm::vec2 pos, glm::vec2 size, Texture2D sprite)
    : GameObject(pos, size, sprite, COLOR_F_LAVA)
{
    m_RenderDepth = -1.0f;
}

void Lava::SetupRigidBody() {
    if (m_RigidBody != nullptr) return;

    glm::vec2 bodyCenter = m_Position + m_Size / 2.f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position = PhysicsUtils::PixelsToMeters(bodyCenter);
    m_RigidBody = physicsWorld.CreateBody(&bodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(PhysicsUtils::PixelsToMeters(m_Size.x / 2.0f),
                      PhysicsUtils::PixelsToMeters(m_Size.y / 2.0f));

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = 0.0f;
    fixtureDef.friction = 0.0f;
    fixtureDef.restitution = 0.0f;
    fixtureDef.filter.categoryBits = F_LAVA;

    m_RigidBody->CreateFixture(&fixtureDef);
}

void Lava::Draw(SpriteRenderer& renderer)
{
    renderer.SetShader(ResourceManager::GetShader("spritelava"));
    renderer.DrawSprite(m_Sprite, m_RenderPosition, m_Size, m_Rotation, m_Color);
}