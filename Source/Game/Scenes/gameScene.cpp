#include <Engine/dogpch.h>
#include "gameScene.h"
#include "titleScene.h"
#include "pauseScene.h"
#include "../Scripts/spawnRoom.h"
#include "../Scripts/cameraController.h"

static GameScene gameSceneInstance;

Dog::Scene* GameScene::GetInstance()
{
	return &gameSceneInstance;
}

void GameScene::Init()
{
	Dog::Entity cameraController = CreateEntity();
	cameraController.AddComponent<Dog::NativeScriptComponent>().Bind<CameraControllerScript>();

	Dog::Entity runGame = CreateEntity();
	auto& script = runGame.AddComponent<Dog::NativeScriptComponent>();
	script.Bind<SpawnRoomScript>();
	script.SetVariable("filename", std::string("Game/Assets/Maps/kk.json"));
}

void GameScene::Update(float dt)
{
	if (Dog::Input::GetMouseTriggered(Dog::Mouse::LEFT)) {
		Dog::SceneManager::SetNextScene(TitleScene::GetInstance());
	}
}

void GameScene::Render(float dt, float ct, float itf)
{
	Dog::Renderer::Clear(0.5f, 0.0f, 0.0f, 1.0f);
}

void GameScene::Exit()
{
}	