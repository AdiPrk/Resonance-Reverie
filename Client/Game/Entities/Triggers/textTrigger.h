#pragma once

#include <PCH/pch.h>

#include "../gameObject.h"

class Texture2D;

class TextTrigger : public GameObject
{
public:
    // constructor
    TextTrigger(glm::vec2 pos, glm::vec2 size, Texture2D sprite, const std::string& text);

    void Draw(SpriteRenderer& renderer, TextRenderer& textRenderer, float dt) override;

private:

    std::string m_Text;
};