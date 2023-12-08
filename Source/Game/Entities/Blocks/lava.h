#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"

class Texture2D;

class Lava : public GameObject
{
public:
    // constructor
    Lava(glm::vec2 pos, glm::vec2 size, Texture2D sprite);
    void SetupRigidBody() override;

    void Draw(float dt) override;
};