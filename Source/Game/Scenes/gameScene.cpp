#include <Engine/dogpch.h>
#include "gameScene.h"
#include "titleScene.h"

static GameScene gameSceneInstance;

Dog::Scene* GameScene::GetInstance()
{
	return &gameSceneInstance;
}

void GameScene::Init()
{
	Dog::Entity dogEntity;
	Dog::Entity catEntity;

	dogEntity = CreateEntity("o.o");
	catEntity = CreateEntity("v.v");

	class DogController : public Dog::ScriptableEntity
	{
	public:
		void OnCreate()
		{
			auto& position = GetComponent<Dog::TransformComponent>().position;
			position.x = Dog::RandomRangeFloat(0.0f, 500.0f);
			position.y = Dog::RandomRangeFloat(0.0f, 500.0f);
		}

		void OnUpdate(float dt)
		{
			auto& position = GetComponent<Dog::TransformComponent>().position;

			float speed = 250.0f;

			if (Dog::Input::GetKeyDown(Dog::Key::A))
				position.x -= speed * dt;
			if (Dog::Input::GetKeyDown(Dog::Key::D))
				position.x += speed * dt;
			if (Dog::Input::GetKeyDown(Dog::Key::W))
				position.y -= speed * dt;
			if (Dog::Input::GetKeyDown(Dog::Key::S))
				position.y += speed * dt;
		}
	};

	class CatController : public Dog::ScriptableEntity
	{
	public:
		void OnUpdate(float dt)
		{
			auto& position = GetComponent<Dog::TransformComponent>().position;
			auto& dogPosition = GetEntity("o.o").GetComponent<Dog::TransformComponent>().position;

			position = dogPosition + glm::vec2(50, 50);
		}
	};

	catEntity.AddComponent<Dog::NativeScriptComponent>().Bind<CatController>();
	dogEntity.AddComponent<Dog::NativeScriptComponent>().Bind<DogController>();

	dogEntity.AddComponent<Dog::SpriteRendererComponent>("circle");
	catEntity.AddComponent<Dog::QuadRendererComponent>(glm::vec4(1.0f, 0.0f, 0.f, 1.0f));

	printf("game init\n");
}

void GameScene::Update(float dt)
{
	if (Dog::Input::GetMouseTriggered(Dog::Mouse::LEFT)) {
		Dog::SceneManager::SetNextScene(TitleScene::GetInstance());
	}

	printf("game update\n");
}

void GameScene::Render(float dt, float ct, float itf)
{
	Dog::Renderer::Clear(0.5f, 0.0f, 0.0f, 1.0f);

	printf("game render\n");
}

void GameScene::Exit()
{
	printf("game exit\n");
}