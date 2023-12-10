#include <Engine/PCH/pch.h>
#include "renderer.h"
#include "Sprites/spriteRenderer.h"
#include "Text/textRenderer.h"
#include "Effects/postProcessor.h"
#include "Camera/camera.h"
#include <Engine/Graphics/Shader/shader.h>
#include <Engine/ResourceManager/resourceManager.h>

namespace Dog {

    TextRenderer* Renderer::m_TextRenderer = nullptr;
    SpriteRenderer* Renderer::m_SpriteRenderer = nullptr;
    PostProcessor* Renderer::m_PostProcessor = nullptr;
    Camera Renderer::m_Camera;;
    Shader Renderer::m_ActiveShader;
    glm::vec4 Renderer::m_ClearColor{ 0, 0, 0, 1 };

    void Renderer::Init(unsigned int width, unsigned int height)
    {
        Shader::SetupUBO();

        ResourceManager::LoadTexturesFromDirectory("Engine/Assets/Images/");
        ResourceManager::LoadShadersFromDirectory("Engine/Assets/Shaders/");

        m_PostProcessor = new PostProcessor(width, height);
        m_SpriteRenderer = new SpriteRenderer();
        m_TextRenderer = new TextRenderer();
        m_TextRenderer->Load("Engine/Assets/Fonts/OCRAEXT.TTF", 60);

        SetShader("defaultsprite");

        glm::mat4 projection = glm::ortho(0.f, (float)width, (float)height, 0.f, -1.0f, 1.0f);
        Shader::SetProjectionUBO(projection);

        m_Camera = Camera({ width * 0.5f, height * 0.5f });
        m_Camera.UpdateUniforms();
    }

    void Renderer::SetShader(Shader& shader)
    {
        m_ActiveShader = shader.Use();
    }

    void Renderer::SetShader(const std::string& shaderName)
    {
        m_ActiveShader = ResourceManager::GetShader(shaderName).Use();
    }

    void Renderer::LoadText(const std::string& fontPath, unsigned int fontSize)
    {
        m_TextRenderer->Load(fontPath, fontSize);
    }

    void Renderer::Clear(const glm::vec4& color)
    {
        m_ClearColor = color;
    }

    void Renderer::Clear(float r, float g, float b, float a)
    {
        m_ClearColor = glm::vec4(r, g, b, a);
    }

    void Renderer::ClearColorAndDepthBuffer() {
        glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::DrawQuad(glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color)
    {
        SetShader("defaultsprite");
        m_SpriteRenderer->DrawSprite("square", position, size, rotation, color);
    }

    void Renderer::DrawSprite(const std::string& textureName, glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color, glm::vec2 repetition)
    {
        m_SpriteRenderer->DrawSprite(textureName, position, size, rotation, color, repetition);
    }

    void Renderer::DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color, glm::vec2 repetition)
    {
        m_SpriteRenderer->DrawSprite(texture, position, size, rotation, color, repetition);
    }

    void Renderer::DrawSpriteFrame(Texture2D& texture, unsigned frame, glm::vec2 position, glm::vec2 size, float rotation, glm::vec4 color, glm::vec2 repetition)
    {
        m_SpriteRenderer->DrawSpriteFrame(texture, frame, position, size, rotation, color, repetition);
    }

    void Renderer::DrawLine(glm::vec2 p1, glm::vec2 p2, float thickness, const std::string& textureName)
    {
        m_SpriteRenderer->DrawLine(p1, p2, thickness, textureName);
    }

    void Renderer::DrawLine(glm::vec2 p1, glm::vec2 p2, float thickness, Texture2D& texture)
    {
        m_SpriteRenderer->DrawLine(p1, p2, thickness, texture);
    }

    void Renderer::RenderText(const std::string& text, float x, float y, float scale, bool centered, glm::vec3 color, bool diagetic)
    {
        m_TextRenderer->RenderText(text, x, y, scale, centered, color, diagetic);
    }

    void Renderer::BeginPostProcessing()
    {
        m_PostProcessor->BeginRender();
    }

    void Renderer::UpdatePostProcessing(float dt)
    {
        m_PostProcessor->Update(dt);
    }

    void Renderer::EndPostProcessing()
    {
        m_PostProcessor->EndRender();
        m_PostProcessor->Render();
    }

    void Renderer::Update(float dt)
    {
        Shader::iTime += dt;
        Shader::SetTimeUBO(Shader::iTime);
        UpdatePostProcessing(dt);
    }
}