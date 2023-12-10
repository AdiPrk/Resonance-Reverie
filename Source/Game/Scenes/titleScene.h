#pragma once

#include <Engine/dogpch.h>

// Title scene, used in game.
class TitleScene : public Dog::Scene
{
public:
	TitleScene() : Dog::Scene("TitleScene") {}
	static Dog::Scene* GetInstance();

private:
	void Init() override;
	void Update(float dt) override;
	void Render(float dt, float ct, float itf) override;
	void Exit() override;
};
