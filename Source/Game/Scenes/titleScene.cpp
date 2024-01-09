#include <Engine/dogpch.h>
#include "titleScene.h"
#include "gameScene.h"
#include "../Scripts/startScript.h"

static TitleScene sceneInstance;

Dog::Scene* TitleScene::GetInstance()
{
	return &sceneInstance;
}

void TitleScene::Init()
{
	Dog::ResourceManager::LoadShadersFromDirectory("Game/Assets/Shaders");
	Dog::ResourceManager::LoadTexturesFromDirectory("Game/Assets/Images");

	Dog::Entity titleEntity = CreateEntity("title");
	titleEntity.AddComponent<Dog::TextRendererComponent>("Resonance Reverie");
	titleEntity.GetComponent<Dog::TransformComponent>().Position = glm::vec2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.2f);

	Dog::Entity startEntity = CreateEntity("start");
	startEntity.AddComponent<Dog::NativeScriptComponent>().Bind<StartEntityScript>();
	startEntity.AddComponent<Dog::TextRendererComponent>("Start by Left Clicking!").Scale = 0.4f;
	startEntity.GetComponent<Dog::TransformComponent>().Position = glm::vec2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.75f);
}

void TitleScene::Update(float dt)
{
}

void TitleScene::Render(float dt, float ct, float itf)
{
	Dog::Renderer::Clear(0.0f, 0.0f, 0.0f, 1.0f);
	
	Dog::Renderer::SetShader("background");
	Dog::Renderer::DrawSprite("square", { 0, 0 }, { WINDOW_WIDTH, WINDOW_HEIGHT });
}

void TitleScene::Exit()
{
}
