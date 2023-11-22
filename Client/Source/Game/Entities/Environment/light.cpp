#include <PCH/pch.h>

#include "light.h"
#include <Source/Graphics/Texture/texture.h>
#include <Source/Game/Physics/physicsWorld.h>
#include <Source/ResourceManager/resourceManager.h>

int Light::lightIndex = 0;

// constructor
Light::Light(glm::vec2 pos, float radius, float intensity)
    : m_Radius(radius)
    , m_Intensity(intensity)
{
    m_Position = pos;
}

void Light::Draw(SpriteRenderer& renderer, float dt)
{
    m_Size = glm::vec2(m_Radius, m_Radius);
    m_Rotation = 0.0f;
    m_Color = glm::vec4(1.0f);
    m_RenderPosition -= m_Radius;
    m_Size *= 2.0f;
    m_Color.w = 0.1f;

    //renderer.SetShader(ResourceManager::GetShader("sprite"));
    //renderer.DrawSprite(ResourceManager::GetTexture("circle"), m_RenderPosition, m_Size, m_Rotation, m_Color);
}

void Light::SetLightInfo()
{
    Shader& pfx = ResourceManager::GetShader("sprite");
    std::string liStr = "lights[" + std::to_string(lightIndex) + "]";
    pfx.SetVector2f(liStr + ".position", m_Position);
    pfx.SetFloat(liStr + ".radius", m_Radius);
    pfx.SetFloat(liStr + ".intensity", m_Intensity);
    lightIndex++;
}