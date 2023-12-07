#include <PCH/pch.h>

#include "swingingBlock.h"
#include <Engine/Graphics/Texture/texture.h>
#include <Game/Physics/physicsWorld.h>
#include <Engine/ResourceManager/resourceManager.h>

// constructor
SwingingBlock::SwingingBlock(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D sprite)
    : GameObject(pos, size, rotation, sprite, COLOR_F_BLOCK)
{
}

void SwingingBlock::SetupRigidBody() {
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
    fixtureDef.density = m_Density;
    fixtureDef.friction = 0.2f;
    fixtureDef.restitution = m_Restitution;
    fixtureDef.filter.categoryBits = F_BLOCK;
    m_RigidBody->CreateFixture(&fixtureDef);

    b2BodyDef anchorDef;
    anchorDef.type = b2_staticBody;
    anchorDef.gravityScale = 0.0f;
    anchorDef.fixedRotation = true;
    anchorDef.position = PhysicsUtils::PixelsToMeters(anchorPos);

    b2Body* anchor = physicsWorld.CreateBody(&anchorDef);

    b2FixtureDef anchorFixture;
    anchorFixture.shape = &boxShape;
    m_RigidBody->CreateFixture(&anchorFixture);
    m_RigidBody->SetAngularDamping(1.0f);

    b2DistanceJointDef jd;
    jd.Initialize(anchor, m_RigidBody, anchorDef.position, bodyDef.position);
    jd.collideConnected = false;

    glm::vec2 anchorpos(anchorDef.position.x, anchorDef.position.y);
    glm::vec2 bodypos(bodyDef.position.x, bodyDef.position.y);
    float dist = glm::distance(anchorpos, bodypos);
    
    float len = dist;
    jd.length = len;
    jd.maxLength = len + 0.2f;
    jd.minLength = len - 0.2f;
    b2LinearStiffness(jd.stiffness, jd.damping, 1.0f, 0.7f, jd.bodyA, jd.bodyB);
    physicsWorld.CreateJoint(&jd);
}

void SwingingBlock::Draw(SpriteRenderer& renderer, TextRenderer& textRenderer, float dt)
{
    glm::vec2 anchorRenderSize = { 10.0f, 10.0f };

    // Drawing the block and anchor
    renderer.SetShader(ResourceManager::GetShader("sprite"));
    renderer.DrawSprite(m_Sprite, m_RenderPosition, m_Size, m_Rotation, m_Color);
    renderer.DrawSprite(m_Sprite, anchorPos, anchorRenderSize);

    // Calculating line info

    glm::vec2 blockCenter = m_RenderPosition + m_Size / 2.0f;
    glm::vec2 anchorCenter = anchorPos + anchorRenderSize / 2.0f;

    renderer.SetShader(ResourceManager::GetShader("sprite"));
    renderer.DrawLine(blockCenter, anchorCenter, lineWidth, m_Sprite);
}

void SwingingBlock::SetUpdatedTransform()
{
    m_Position = PhysicsUtils::MetersToPixels(m_RigidBody->GetPosition()) - m_Size / 2.f;
    m_Rotation = m_RigidBody->GetAngle();
}