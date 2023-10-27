#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"

class Texture2D;

class SafeZone : public GameObject
{
public:
    // constructor
    SafeZone(glm::vec2 pos, glm::vec2 size, Texture2D sprite);
    void SetupRigidBody() override;

};