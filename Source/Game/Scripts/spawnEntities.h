#pragma once

#include <Engine/dogpch.h>

class PlayerControllerScript;

void SpawnEntities(const auto& entities, int roomId, bool makeColliders) {
    for (const auto& entity : entities)
    {
        glm::vec2 pos(entity["x"], entity["y"]);
        glm::vec2 size(entity["w"], entity["h"]);

        if (entity["type"] == "1") // player init position
        {
            Dog::Renderer::GetCamera().MoveTo(pos);
            Dog::Renderer::GetCamera().SetScale(0.5f);
            
            auto playerEntity = Dog::GetEntity("player");
            auto& transform = playerEntity.GetComponent<Dog::TransformComponent>();
            transform.SetTransform(pos, size);
        }
        else if (entity["type"] == "3") // solid
        {
            float rotation = entity["rotation"];

            if (entity["dynamic"] == false) {
                Dog::Entity blockEntity = Dog::CreateEntityInGroup(std::to_string(roomId));

                auto& transform = blockEntity.GetComponent<Dog::TransformComponent>();
                transform.SetTransform(pos, size, rotation);
                transform.Depth = 0.0f;

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
                Dog::Entity blockEntity = Dog::CreateEntityInGroup(std::to_string(roomId));

                auto& transform = blockEntity.GetComponent<Dog::TransformComponent>();
                transform.SetTransform(pos, size, rotation);
                transform.Depth = 0.0f;

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