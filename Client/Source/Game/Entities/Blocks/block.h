#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"

class Texture2D;

class Block : public GameObject
{
public:
    // constructor
    Block(glm::vec2 pos, glm::vec2 size, float rotation, Texture2D sprite);
    void SetupRigidBody() override;

    void Draw(SpriteRenderer& renderer) override;
};