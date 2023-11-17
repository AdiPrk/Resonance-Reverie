#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"
#include <Source/Math/rect.h>

class Texture2D;

class DynamicBlock : public GameObject
{
public:
    // constructor
    DynamicBlock(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D sprite);
    void SetupRigidBody() override;

    void Draw(SpriteRenderer& renderer) override;
    void SetUpdatedPosition() override;
    bool GetOverlappingBounds(const Rect& gamebounds) override;
    bool GetOutOfBounds(const Rect& gamebounds) override;
    const Rect& Bounds() override { return m_BoundingRect; }

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