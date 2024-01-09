#pragma once

#include <Engine/dogpch.h>
#include "playerController.h"

class SpawnRoomScript : public Dog::ScriptableEntity
{
public:
    void SpawnEntities(const auto& entities, bool makeColliders) {
        for (const auto& entity : entities)
        {
            glm::vec2 pos(entity["x"], entity["y"]);
            glm::vec2 size(entity["w"], entity["h"]);

            if (entity["type"] == "1") // player init position
            {
                Dog::Renderer::GetCamera().MoveTo(pos);
                Dog::Renderer::GetCamera().SetScale(0.5f);

                Dog::Entity playerEntity = CreateEntity("player");
                auto& transform = playerEntity.GetComponent<Dog::TransformComponent>();
                transform.SetTransform(pos, size);
                
                playerEntity.AddComponent<Dog::NativeScriptComponent>().Bind<PlayerControllerScript>();
            }
            else if (entity["type"] == "3") // solid
            {
                float rotation = entity["rotation"];

                if (entity["dynamic"] == false) {
                    Dog::Entity blockEntity = CreateEntity();

                    auto& transform = blockEntity.GetComponent<Dog::TransformComponent>();
                    transform.SetTransform(pos, size, rotation);

                    //blockEntity.AddComponent<Dog::QuadRendererComponent>(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
                    auto& sprite = blockEntity.AddComponent<Dog::SpriteRendererComponent>("ss16x7tiles");
                    sprite.Repetition = { 25.0f, 25.0f };
                    sprite.Texture.Index = entity["texture"];

                    if (makeColliders) {
                        blockEntity.AddComponent<Dog::RigidbodyComponent>();
                        blockEntity.AddComponent<Dog::BoxColliderComponent>(transform.Scale);
                    }
                }
                else {
                    Dog::Entity blockEntity = CreateEntity();

                    auto& transform = blockEntity.GetComponent<Dog::TransformComponent>();
                    transform.SetTransform(pos, size, rotation);

                    //blockEntity.AddComponent<Dog::QuadRendererComponent>(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
                    auto& sprite = blockEntity.AddComponent<Dog::SpriteRendererComponent>("ss16x7tiles");
                    sprite.Repetition = { 25.0f, 25.0f };
                    sprite.Texture.Index = entity["texture"];

                    if (makeColliders) {
                        auto& rb = blockEntity.AddComponent<Dog::RigidbodyComponent>();
                        rb.Type = Dog::RigidbodyComponent::BodyType::Dynamic;

                        Dog::FixtureDefinition fixture;
                        fixture.BoxSize = transform.Scale;
                        fixture.Restitution = entity["restitution"];
                        fixture.Density = entity["density"];
                        blockEntity.AddComponent<Dog::BoxColliderComponent>(fixture);
                    }
                }
            }
        }
    }

	void OnCreate() {
		std::string filename = GetVariable<std::string>("filename");

        json data;
        std::ifstream file(filename);
        if (!file) {
            std::cout << "Error opening file: " << filename << std::endl;
            return;
        }
        data = json::parse(file);

        for (const json::value_type& element : data)
        {
            if (!element.contains("starting")) continue;
            if (element["starting"] != true) continue;

            SpawnEntities(element["backgroundEntities"], false);
            SpawnEntities(element["foregroundEntities"], true);

            return;
        }


	}

	void OnDestroy() {
		
	}
	
	void OnUpdate(float dt) {
	}
};