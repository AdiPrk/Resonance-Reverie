#include <Engine/dogpch.h>
#include "game.h"

Game::Game(Dog::Window* window)
    : m_Width(window->GetWidth())
    , m_Height(window->GetHeight())
    , m_Window(window)
    , m_ActiveScene("gamescene")
{
    Dog::ResourceManager::LoadTexturesFromDirectory("Game/Assets/Images/");
    Dog::ResourceManager::LoadShadersFromDirectory("Game/Assets/Shaders/");
}

Game::~Game() {}
void Game::Update(float dt) {}

// delta time, current time, interpolation factor
void Game::Render(float dt, float ct, float itf)
{
    Dog::Renderer::Clear({ 0.5f, 0.0f, 0.0f, 1.0f });
    
    // Begin rendering to postprocessing framebuffer
    Dog::Renderer::GetPostProcessor()->Disco = true;
    Dog::Renderer::BeginPostProcessing();

    Dog::Renderer::SetShader("background");
    Dog::Renderer::DrawSprite("square", glm::vec2(0, 0), glm::vec2(m_Width, m_Height));

    Dog::Renderer::DrawQuad({ 550, 150 }, { 80, 80 });
    Dog::Renderer::DrawQuad({ 750, 150 }, { 80, 80 });
    Dog::Renderer::DrawQuad({ 550, 300 }, { 300, 80 });

    // End rendering to postprocessing framebuffer and render the scene
    Dog::Renderer::EndPostProcessing();

    // Draw fps
    Dog::Renderer::RenderText(std::string("fps: ") + std::to_string((1.f / dt)), 50.f, 90.0f, 0.5f);

    // fullscreen toggle
    Dog::Renderer::RenderText("press F to toggle fullscreen", 50.f, (float)m_Height - 70, 0.5f);
    if (Dog::Input::GetKeyTriggered(Dog::Key::F))
        m_Window->ToggleFullscreen();
}