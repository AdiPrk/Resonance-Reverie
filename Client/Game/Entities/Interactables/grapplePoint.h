#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"

class Texture2D;

class GrapplePoint : public GameObject
{
public:
    // constructor
    GrapplePoint(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D sprite);
    void SetupRigidBody() override;

    void Draw(SpriteRenderer& renderer, TextRenderer& textRenderer, float dt) override;

    const float GetRadius() const { return m_Radius; }
private:
    float m_Radius;
};