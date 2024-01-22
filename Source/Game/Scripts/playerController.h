#pragma once

#include <Engine/dogpch.h>
#include "spawnNextRoom.h"

b2FixtureDef standingFixtureDef;
b2FixtureDef crouchingFixtureDef;

class PlayerControllerScript : public Dog::ScriptableEntity
{
public:
	void OnCreate()
	{
		auto& transform = GetComponent<Dog::TransformComponent>();

		//AddComponent<Dog::SpriteRendererComponent>("square");	
		AddComponent<Dog::QuadRendererComponent>(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		
		auto& rb = AddComponent<Dog::RigidbodyComponent>();
		rb.Type = Dog::RigidbodyComponent::BodyType::Dynamic;
		rb.FixedRotation = true;

		auto& collider = AddComponent<Dog::BoxColliderComponent>();
		
		Dog::FixtureDefinition standingHitbox;
		standingHitbox.BoxSize = { transform.Scale.x, transform.Scale.y };
		standingHitbox.Name = "standing";
		SetVariable("standingHitbox", standingHitbox);

		Dog::FixtureDefinition crouchingHitbox;
		crouchingHitbox.BoxSize = { transform.Scale.x, transform.Scale.y * 0.5f };
		crouchingHitbox.Offset = { 0.0f, transform.Scale.y * 0.25f };
		crouchingHitbox.Name = "crouching";
		SetVariable("crouchingHitbox", crouchingHitbox);

		Dog::FixtureDefinition footSensor;
		footSensor.Shape = Dog::FixtureDefinition::ShapeType::Box;
		footSensor.Offset = { 0.f, transform.Scale.y * 0.5f + 3.0f };
		footSensor.BoxSize = { transform.Scale.x * 0.9f, 10.0f };
		footSensor.IsSensor = true;

		Dog::FixtureDefinition leftSensor;
		leftSensor.Shape = Dog::FixtureDefinition::ShapeType::Box;
		leftSensor.Offset = { transform.Scale.x * -0.5f - 3.0f, 0.0f };
		leftSensor.BoxSize = { 10.0f, transform.Scale.y * 0.9f };
		leftSensor.IsSensor = true;

		Dog::FixtureDefinition rightSensor;
		rightSensor.Shape = Dog::FixtureDefinition::ShapeType::Box;
		rightSensor.Offset = { transform.Scale.x * 0.5f + 3.0f, 0.0f };
		rightSensor.BoxSize = { 10.0f, transform.Scale.y * 0.9f };
		rightSensor.IsSensor = true;

		Dog::FixtureDefinition headSensor;
		headSensor.Shape = Dog::FixtureDefinition::ShapeType::Box;
		headSensor.Offset = { 0.f, transform.Scale.y * -0.5f - 2.0f };
		headSensor.BoxSize = { transform.Scale.x * 0.9f, 5.0f };
		headSensor.IsSensor = true;

		collider.AddFixtures({ footSensor, leftSensor, rightSensor, headSensor, standingHitbox });
		SetVariable("standing", true);
	}

	void OnUpdate(float dt)
	{
		auto& rb = GetComponent<Dog::RigidbodyComponent>();
		if (!rb.Body) return;

		auto& collider = GetComponent<Dog::BoxColliderComponent>();
		auto& transform = GetComponent<Dog::TransformComponent>();

		auto velocity = rb.Body->GetLinearVelocity();
		
		float speed = 25.0f;

		if (Dog::Input::GetKeyDown(Dog::Key::A))
			velocity.x -= speed * dt;
		if (Dog::Input::GetKeyDown(Dog::Key::D))
			velocity.x += speed * dt;
		if (Dog::Input::GetKeyDown(Dog::Key::W))
			velocity.y -= speed * dt;
		if (Dog::Input::GetKeyDown(Dog::Key::S))
			velocity.y += speed * dt;

		if (Dog::Input::GetKeyTriggered(Dog::Key::K))
		{
			bool standing = GetVariable<bool>("standing");
			if (standing)
			{
				collider.RemoveFixture("standing");
				collider.AddFixtures(GetVariable<Dog::FixtureDefinition>("crouchingHitbox"));
				SetVariable("standing", false);
			}
			else
			{
				collider.RemoveFixture("crouching");
				collider.AddFixtures(GetVariable<Dog::FixtureDefinition>("standingHitbox"));
				SetVariable("standing", true);
			}
		}

		rb.Body->SetLinearVelocity(velocity);

		glm::vec2 pos = rb.GetPosition();
		collider.Bounds.left = pos.x - transform.Scale.x * 0.5f;
		collider.Bounds.top = pos.y - transform.Scale.y * 0.5f;
		collider.Bounds.SetScale(transform.Scale);

		/*printf("pos: %f, %f\n", pos.x, pos.y);
		if (GetVariable<bool>("standing")) {
			collider.Bounds.left = pos.x - transform.Scale.x * 0.5f;
			collider.Bounds.top = pos.y - transform.Scale.y * 0.5f;
			collider.Bounds.SetScale(transform.Scale);
		}
		else {
			collider.Bounds.left = pos.x - transform.Scale.x * 0.5f;
			collider.Bounds.top = pos.y + transform.Scale.y * 0.25f;
			collider.Bounds.SetScale({ transform.Scale.x, transform.Scale.y * 0.5f });
		}*/

		auto& b = collider.Bounds;
		if (!GetVariable<Dog::Rect>("roomBounds").overlaps(b)) {
			Dog::Entity newRoom = CreateEntity();
			auto& script = newRoom.AddComponent<Dog::NativeScriptComponent>().Bind<SpawnNextRoomScript>();
			script.SetVariable("currentRoomId", GetVariable<int>("roomId"));
			script.SetVariable("filename", std::string("Game/Assets/Maps/map.json"));
		}

		// GetVariable<Dog::Rect>("roomBounds").PrintBounds();
	}
};