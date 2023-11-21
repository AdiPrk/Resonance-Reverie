#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"

class Texture2D;

class GreyEnemy : public GameObject
{
public:
    // constructor
    GreyEnemy(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec4 color = glm::vec4(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    void SetupRigidBody() override;

    void Update(float dt) override;
    void SetUpdatedTransform() override;

private:

};