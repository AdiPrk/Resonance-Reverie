#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"
#include "../Player/player.h"

class Texture2D;

class Light : public GameObject
{
public:
    // constructor
    Light(glm::vec2 pos, float radius, float intensity);

    void Draw(SpriteRenderer& renderer) override;
    void SetLightInfo() override;

    float m_Radius;
    float m_Intensity;

    static int lightIndex;

private:
    friend Player;
};