#pragma once

#include <Engine/dogpch.h>

// Title scene, used in game.
class GameScene : public Dog::Scene
{
public:
	GameScene() : Dog::Scene("GameScene") {}

	void Init() override;
	void Update(float dt) override;
	void Render(float dt, float ct, float itf) override;
	void Exit() override;

	static Dog::Scene* GetInstance();
};
