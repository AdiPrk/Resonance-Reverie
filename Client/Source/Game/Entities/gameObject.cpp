#include <PCH/pch.h>
#include "gameObject.h"
#include <Source/Game/Physics/physicsWorld.h>
#include <Source/Graphics/ResourceManager/resourceManager.h>

GameObject::GameObject()
    : m_Position(0.0f, 0.0f)
    , m_Size(1.0f, 1.0f)
    , m_Velocity(0.0f)
    , m_Color(0.1f, 1.0f, 0.1f, 1.0f)
    , m_Rotation(0.0f)
    , m_Sprite()
    , m_PrevPosition(m_Position)
    , m_RenderPosition(m_Position)
    , m_RenderDepth(0.0f)
    , m_RigidBody(nullptr)
{
}

GameObject::GameObject(Texture2D sprite, glm::vec4 color)
    : m_Position(0.0f, 0.0f)
    , m_Size(1.0f, 1.0f)
    , m_Velocity(0.0f)
    , m_Color(color)
    , m_Rotation(0.0f)
    , m_Sprite(sprite)
    , m_PrevPosition(m_Position)
    , m_RenderPosition(m_Position)
    , m_RenderDepth(0.0f)
    , m_RigidBody(nullptr)
{
}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D sprite, glm::vec4 color, glm::vec2 velocity)
    : m_Position(pos)
    , m_Size(size)
    , m_Velocity(velocity)
    , m_Color(color)
    , m_Rotation(rotation)
    , m_Sprite(sprite)
    , m_PrevPosition(m_Position)
    , m_RenderPosition(m_Position)
    , m_RenderDepth(0.0f)
    , m_RigidBody(nullptr)
{
}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec4 color, glm::vec2 velocity)
    : m_Position(pos)
    , m_Size(size)
    , m_Velocity(velocity)
    , m_Color(color)
    , m_Rotation(0.0f)
    , m_Sprite(sprite)
    , m_PrevPosition(m_Position)
    , m_RenderPosition(m_Position)
    , m_RenderDepth(0.0f)
    , m_RigidBody(nullptr)
{
}

GameObject::~GameObject()
{
    if (m_RigidBody != nullptr) {
        physicsWorld.DestroyBody(m_RigidBody);
        m_RigidBody = nullptr;
    }
}

void GameObject::Draw(SpriteRenderer& renderer)
{
    renderer.SetShader(ResourceManager::GetShader("sprite"));
    renderer.DrawSprite(m_Sprite, m_RenderPosition, m_Size, m_Rotation, m_Color);
}

void GameObject::setPreviousPosition()
{
    m_PrevPosition = m_Position;
}

void GameObject::setLerpedPosition(float t)
{
    m_RenderPosition.x = std::lerp(m_PrevPosition.x, m_Position.x, t);
    m_RenderPosition.y = std::lerp(m_PrevPosition.y, m_Position.y, t);
}
