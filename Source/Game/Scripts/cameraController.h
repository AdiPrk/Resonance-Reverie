#pragma once

#include <Engine/dogpch.h>

class CameraControllerScript : public Dog::ScriptableEntity
{
public:
	void OnCreate()
	{
	}

	void OnUpdate(float dt)
	{
		auto player = GetEntity("player");
		auto playerTransform = player.GetComponent<Dog::TransformComponent>();
		
		auto& camera = Dog::Renderer::GetCamera();
		camera.MoveTo(playerTransform.Position);
		camera.SetScale(1.25f);
	}
};