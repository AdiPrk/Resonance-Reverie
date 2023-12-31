#pragma once

#include <PCH/pch.h>

#include <Engine/Graphics/Texture/texture.h>
#include <Engine/Graphics/Renderer/Sprites/spriteAnimator.h>

// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
    // constructor(s)
    GameObject();
    GameObject(Dog::Texture2D sprite, glm::vec4 color = glm::vec4(1.0f));
    GameObject(glm::vec2 pos, glm::vec2 size, float rotation, Dog::Texture2D sprite, glm::vec4 color = glm::vec4(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    GameObject(glm::vec2 pos, glm::vec2 size, Dog::Texture2D sprite, glm::vec4 color = glm::vec4(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    ~GameObject();

    virtual void Update(float dt) {};
    virtual void Draw(float dt);
    virtual void SetUpdatedTransform() {};
    virtual void SetupRigidBody() {};
    virtual bool GetOverlappingBounds(const Dog::Rect& gamebounds) const { return true; }
    virtual bool GetOutOfBounds(const Dog::Rect& gamebounds) const { return false; }
    virtual void SetLightInfo() {};
    void setPreviousPosition();
    void setLerpedPosition(float t);

    glm::vec2& GetRenderPosition() { return m_RenderPosition; }
    glm::vec2& GetPosition() { return m_Position; }
    float& GetRotation() { return m_Rotation; }
    glm::vec4& GetColor() { return m_Color; }
    glm::vec2& GetSize() { return m_Size; }
    float& GetRenderDepth() { return m_RenderDepth; }
    Dog::Texture2D& GetSprite() { return m_Sprite; }
    b2Body* GetBody() { return m_RigidBody; }

protected:

    b2Body* m_RigidBody;
    float m_RenderDepth;
    float m_Rotation;
    glm::vec2 m_Position, m_Size, m_Velocity;
    glm::vec2 m_PrevPosition, m_RenderPosition;
    glm::vec4 m_Color;
    Dog::Texture2D m_Sprite;
    Dog::SpriteAnimator m_Animator;
};