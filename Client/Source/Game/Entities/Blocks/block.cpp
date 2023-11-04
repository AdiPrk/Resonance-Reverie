#include <PCH/pch.h>

#include "block.h"
#include <Source/Graphics/Texture/texture.h>
#include <Source/Game/Physics/physicsWorld.h>
#include <Source/Graphics/ResourceManager/resourceManager.h>

// constructor
Block::Block(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D sprite)
    : GameObject(pos, size, rotation, sprite, COLOR_F_BLOCK)
{
}

void Block::SetupRigidBody() {
    if (m_RigidBody != nullptr) return;

    glm::vec2 bodyCenter = m_Position + m_Size / 2.f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position = PhysicsUtils::PixelsToMeters(bodyCenter);
    bodyDef.angle = m_Rotation;
    m_RigidBody = physicsWorld.CreateBody(&bodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(PhysicsUtils::PixelsToMeters(m_Size.x / 2.0f),
                      PhysicsUtils::PixelsToMeters(m_Size.y / 2.0f));

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = 0.0f;
    fixtureDef.friction = 0.4f;
    fixtureDef.restitution = 0.0f;
    fixtureDef.filter.categoryBits = F_BLOCK;

    m_RigidBody->CreateFixture(&fixtureDef);
}

void Block::Draw(SpriteRenderer& renderer)
{
    renderer.SetShader(ResourceManager::GetShader("sprite"));
    renderer.DrawSprite(m_Sprite, m_RenderPosition, m_Size, m_Rotation, m_Color, { 25.0f, 25.0f });
}