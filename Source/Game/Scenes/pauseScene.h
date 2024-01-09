#pragma once

#include <Engine/dogpch.h>

// Title scene, used in game.
class PauseScene : public Dog::Scene
{
public:
	PauseScene() : Dog::Scene("PauseScene") {}

	void Init() override;
	void Update(float dt) override;
	void Render(float dt, float ct, float itf) override;
	void Exit() override;

	static Dog::Scene* GetInstance();
};
