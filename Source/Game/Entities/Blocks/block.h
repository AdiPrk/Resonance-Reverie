#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"

class Texture2D;

class Block : public GameObject
{
public:
    // constructor
    Block(glm::vec2 pos, glm::vec2 size, float rotation, Dog::Texture2D sprite, int index);
    void SetupRigidBody() override;

    void Draw(float dt) override;

private:
    int m_Index;
};