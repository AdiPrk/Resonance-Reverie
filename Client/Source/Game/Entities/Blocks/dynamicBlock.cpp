#include <PCH/pch.h>

#include "dynamicBlock.h"
#include <Source/Graphics/Texture/texture.h>
#include <Source/Game/Physics/physicsWorld.h>
#include <Source/Graphics/ResourceManager/resourceManager.h>

// constructor
DynamicBlock::DynamicBlock(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D sprite)
    : GameObject(pos, size, rotation, sprite, COLOR_F_BLOCK)
    , m_BoundingRect(0, 0, 25.f, 25.f)
{
}

void DynamicBlock::SetupRigidBody() {
    if (m_RigidBody != nullptr) return;

    glm::vec2 bodyCenter = m_Position + m_Size / 2.f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = PhysicsUtils::PixelsToMeters(bodyCenter);
    m_RigidBody = physicsWorld.CreateBody(&bodyDef);
    m_RigidBody->SetGravityScale(0.4f);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(PhysicsUtils::PixelsToMeters(m_Size.x / 2.0f),
                      PhysicsUtils::PixelsToMeters(m_Size.y / 2.0f));

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = m_Density;
    fixtureDef.friction = 0.2f;
    fixtureDef.restitution = m_Restitution;
    fixtureDef.filter.categoryBits = F_BLOCK;
    fixtureDef.filter.maskBits = 0xFFFF ^ F_LAVA; // Collides with everything except lava
    m_RigidBody->CreateFixture(&fixtureDef);
}

void DynamicBlock::Draw(SpriteRenderer& renderer)
{
    renderer.SetShader(ResourceManager::GetShader("sprite"));
    renderer.DrawSprite(m_Sprite, m_RenderPosition, m_Size, m_Rotation, m_Color);
}

void DynamicBlock::SetUpdatedPosition()
{
    m_Position = PhysicsUtils::MetersToPixels(m_RigidBody->GetPosition()) - m_Size / 2.f;
    m_Rotation = m_RigidBody->GetAngle();

    m_BoundingRect.left = m_Position.x;
    m_BoundingRect.top = m_Position.y;
    m_BoundingRect.SetScale(m_Size);
}

bool DynamicBlock::GetOutOfBounds(const Rect& gamebounds)
{
    return !m_BoundingRect.bordersOverlap(gamebounds);
}