#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"

class Texture2D;

class DynamicBlock : public GameObject
{
public:
    // constructor
    DynamicBlock(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D sprite);
    void SetupRigidBody() override;

    void Draw(SpriteRenderer& renderer) override;
    void SetUpdatedPosition() override;

    void SetRestitution(float restitution) {
        m_Restitution = restitution;
    }

private:
    float m_Restitution = 0.0f;
};