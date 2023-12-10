#include <PCH/pch.h>

#include "textTrigger.h"
#include <Engine/Graphics/Texture/texture.h>
#include <Game/Physics/physicsWorld.h>
#include <Engine/ResourceManager/resourceManager.h>
#include <Engine/Graphics/Renderer/renderer.h>

// constructor
TextTrigger::TextTrigger(glm::vec2 pos, glm::vec2 size, Dog::Texture2D sprite, const std::string& text)
    : GameObject(pos, size, sprite)
    , m_Text(text)
{
}

void TextTrigger::Draw(float dt)
{
    //Renderer::SetShader("sprite");
    //Renderer::DrawSprite(m_Sprite, m_RenderPosition, m_Size, m_Rotation, m_Color, { 25.0f, 25.0f });
    //float textX = m_Position.x + m_Size.x * 0.5;
    //float textY = m_Position.y + m_Size.y * 0.5;
    //float fontSize = m_Size.x;
    //Renderer::RenderText(m_Text, textX, textY - 5, fontSize, true, { 1, 1, 1 }, true);
    //Renderer::RenderText(m_Text, textX, textY, fontSize, true, { 0, 0, 0 }, true);
    int numParticles = 10000;
    Dog::Renderer::RenderText(std::to_string(numParticles) + " particles", 800, 450, 0.5f, true, glm::vec3(1), true);
}