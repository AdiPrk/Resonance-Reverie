#pragma once

#include <Engine/dogpch.h>
#include <Game/Scenes/gameScene.h>

class StartEntityScript : public Dog::ScriptableEntity
{
public:
	void OnCreate() {
		Dog::Renderer::GetCamera().SetScaleInstant(1.25f);
		Dog::Renderer::GetCamera().SetScale(1.0f);
		Dog::Renderer::GetCamera().MoveTo({ WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f });
		//Dog::GetWindow()->SetFullscreen();
	}

	void OnUpdate(float dt) {
		if (Dog::Input::GetMouseTriggered(Dog::Mouse::LEFT)) {
			Dog::SceneManager::SetNextScene(GameScene::GetInstance());
		}
	}
};