#include <Engine/dogpch.h>
#include "titleScene.h"
#include "gameScene.h"

static TitleScene sceneInstance;

Dog::Scene* TitleScene::GetInstance()
{
	return &sceneInstance;
}

void TitleScene::Init()
{
	Dog::Entity titleEntity;
	Dog::Entity startEntity;

	titleEntity = CreateEntity("o.o");
	startEntity = CreateEntity("v.v");

	class StartEntityScript : public Dog::ScriptableEntity
	{
	public:
		void OnUpdate(float dt) {
			if (Dog::Input::GetMouseTriggered(Dog::Mouse::LEFT)) {
				Dog::SceneManager::SetNextScene(GameScene::GetInstance());
			}
		}
	};

	startEntity.AddComponent<Dog::NativeScriptComponent>().Bind<StartEntityScript>();

	titleEntity.AddComponent<Dog::TextRendererComponent>("Title Sceen");
	titleEntity.GetComponent<Dog::TransformComponent>().position = glm::vec2(500, 200);

	startEntity.AddComponent<Dog::TextRendererComponent>("Start by Left Clicking!");
	startEntity.GetComponent<Dog::TransformComponent>().position = glm::vec2(500, 500);

	printf("title init\n");
}

void TitleScene::Update(float dt)
{
	printf("title update\n");
}

void TitleScene::Render(float dt, float ct, float itf)
{
	Dog::Renderer::Clear(0.5f, 0.0f, 0.0f, 1.0f);
	printf("title render\n");
}

void TitleScene::Exit()
{
	printf("title exit\n");
}
