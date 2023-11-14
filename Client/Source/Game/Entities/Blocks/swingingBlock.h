#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"

class Texture2D;

class SwingingBlock : public GameObject
{
public:
    // constructor
    SwingingBlock(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D sprite);
    void SetupRigidBody() override;

    void Draw(SpriteRenderer& renderer) override;
    void SetUpdatedPosition() override;

    void SetRestitution(float restitution) {
        m_Restitution = restitution;
    }
    void SetDensity(float density) {
        m_Density = density;
    }

    glm::vec2 anchorPos;
    float lineWidth;

private:
    float m_Restitution = 0.0f;
    float m_Density = 0.0f;
};