#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"
#include <Engine/Math/rect.h>

class Texture2D;

class DynamicBlock : public GameObject
{
public:
    // constructor
    DynamicBlock(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D sprite);
    void SetupRigidBody() override;

    void Draw(SpriteRenderer& renderer, float dt) override;
    void SetUpdatedTransform() override;
    bool GetOverlappingBounds(const Rect& gamebounds) const override;
    bool GetOutOfBounds(const Rect& gamebounds) const override;

    void SetRestitution(float restitution) {
        m_Restitution = restitution;
    }
    void SetDensity(float density) {
        m_Density = density;
    }

private:
    float m_Restitution = 0.0f;
    float m_Density = 0.0f;
    Rect m_BoundingRect;
};