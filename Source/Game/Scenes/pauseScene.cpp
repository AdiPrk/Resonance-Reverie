#include <Engine/dogpch.h>
#include "PauseScene.h"
#include "titleScene.h"

static PauseScene pauseSceneInstance;

Dog::Scene* PauseScene::GetInstance()
{
	return &pauseSceneInstance;
}

void PauseScene::Init()
{
	Dog::Entity dogEntity;
	Dog::Entity mouseEntity;

	dogEntity = CreateEntity("o.o");
	mouseEntity = CreateEntity("w.w");

	class DogController : public Dog::ScriptableEntity
	{
	public:
		void OnCreate()
		{
			auto& position = GetComponent<Dog::TransformComponent>().Position;
			position.x = Dog::RandomRangeFloat(0.0f, 500.0f);
			position.y = Dog::RandomRangeFloat(0.0f, 500.0f);
		}

		void OnUpdate(float dt)
		{
			auto& position = GetComponent<Dog::TransformComponent>().Position;

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

	dogEntity.AddComponent<Dog::NativeScriptComponent>().Bind<DogController>();

	dogEntity.AddComponent<Dog::SpriteRendererComponent>("square");


	auto& rb = dogEntity.AddComponent<Dog::RigidbodyComponent>();
	//rb.Type = Dog::RigidbodyComponent::BodyType::Dynamic;

	Dog::FixtureDefinition fixture;
	fixture.BoxSize = dogEntity.GetComponent<Dog::TransformComponent>().Scale;
	//fixture.CircleRadius = dogEntity.GetComponent<Dog::TransformComponent>().Scale.x * 0.5f;
	//fixture.Shape = Dog::FixtureDefinition::ShapeType::Circle;
	auto& box = dogEntity.AddComponent<Dog::BoxColliderComponent>(fixture);

	mouseEntity.AddComponent<Dog::QuadRendererComponent>(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	mouseEntity.GetComponent<Dog::TransformComponent>().Position = glm::vec2(450, 100);
	dogEntity.GetComponent<Dog::TransformComponent>().Position = glm::vec2(450, 400);
	auto& rb2 = mouseEntity.AddComponent<Dog::RigidbodyComponent>();
	rb2.Type = Dog::RigidbodyComponent::BodyType::Dynamic;

	Dog::FixtureDefinition fixture2;
	fixture2.BoxSize = mouseEntity.GetComponent<Dog::TransformComponent>().Scale;
	auto& crate = mouseEntity.AddComponent<Dog::BoxColliderComponent>(fixture2);
}

void PauseScene::Update(float dt)
{
	if (Dog::Input::GetMouseTriggered(Dog::Mouse::LEFT)) {
		Dog::SceneManager::SetNextScene(TitleScene::GetInstance());
	}
}

void PauseScene::Render(float dt, float ct, float itf)
{
	Dog::Renderer::Clear(0.5f, 0.0f, 0.0f, 1.0f);
}

void PauseScene::Exit()
{
}