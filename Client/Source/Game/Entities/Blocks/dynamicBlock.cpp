#include <PCH/pch.h>

#include "dynamicBlock.h"
#include <Source/Graphics/Texture/texture.h>
#include <Source/Game/Physics/physicsWorld.h>
#include <Source/Graphics/ResourceManager/resourceManager.h>

// constructor
DynamicBlock::DynamicBlock(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D sprite)
    : GameObject(pos, size, rotation, sprite, COLOR_F_BLOCK)
{
}

void DynamicBlock::SetupRigidBody() {
    if (m_RigidBody != nullptr) return;

    glm::vec2 bodyCenter = m_Position + m_Size / 2.f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody; // or b2_staticBody, b2_kinematicBody, depending on your needs
    bodyDef.position = PhysicsUtils::PixelsToMeters(bodyCenter);
    m_RigidBody = physicsWorld.CreateBody(&bodyDef);
    //m_RigidBody->SetFixedRotation(true);
    m_RigidBody->SetGravityScale(0.4f);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(PhysicsUtils::PixelsToMeters(m_Size.x / 2.0f),
        PhysicsUtils::PixelsToMeters(m_Size.y / 2.0f));

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.2f;
    fixtureDef.restitution = m_Restitution;
    fixtureDef.filter.categoryBits = F_BLOCK;
    m_RigidBody->CreateFixture(&fixtureDef);

    if (m_Rotation == 0) {
        b2BodyDef anchorDef;
        anchorDef.type = b2_staticBody;
        anchorDef.gravityScale = 0.0f;
        anchorDef.fixedRotation = true;

        b2Body* anchor = physicsWorld.CreateBody(&anchorDef);

        b2FixtureDef anchorFixture;
        anchorFixture.shape = &boxShape;
        m_RigidBody->CreateFixture(&anchorFixture);

        b2DistanceJointDef jd;
        jd.Initialize(anchor, m_RigidBody, b2Vec2(bodyDef.position.x, bodyDef.position.y), bodyDef.position);
        jd.collideConnected = false;
        jd.length = 2.0f;
        jd.maxLength = 100.0f;
        jd.minLength = 0.0f;
        b2LinearStiffness(jd.stiffness, jd.damping, 1.0f, 0.7f, jd.bodyA, jd.bodyB);
        physicsWorld.CreateJoint(&jd);

    }
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
}