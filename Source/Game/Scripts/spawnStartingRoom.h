#pragma once

#include <Engine/dogpch.h>
#include "playerController.h"
#include "spawnEntities.h"

class SpawnStartingRoomScript : public Dog::ScriptableEntity
{
public:
	void OnCreate() {
		std::string filename = GetVariable<std::string>("filename");

        json data;
        std::ifstream file(filename);
        if (!file) {
            std::cout << "Error opening file: " << filename << std::endl;
            return;
        }
        data = json::parse(file);

        Dog::Entity playerEntity = CreateEntity("player");
        playerEntity.AddComponent<Dog::NativeScriptComponent>().Bind<PlayerControllerScript>();
        playerEntity.GetComponent<Dog::TransformComponent>().Depth = 1.0f;

        for (const json::value_type& element : data)
        {
            if (!element.contains("starting")) continue;
            if (element["starting"] != true) continue;

            Dog::Rect roomBounds = { element["x"], element["y"], element["w"], element["h"] };
            Dog::Renderer::GetCamera().SetMovementBounds(roomBounds);
            int roomId = element["id"];

            printf("createin %i\n", roomId);

            //Dog::Entity bg = CreateEntityInGroup(std::to_string(roomId));
            //bg.AddComponent<Dog::QuadRendererComponent>(glm::vec4(0.2f, 0.2f, 0.2f, 1.f));
            //auto& bgTransform = bg.GetComponent<Dog::TransformComponent>();
            //bgTransform.SetTransform({ roomBounds.left, roomBounds.top }, { roomBounds.width, roomBounds.height });
            //bgTransform.Depth = 0.0f;

            SpawnEntities(element["backgroundEntities"], roomId, false);
            SpawnEntities(element["foregroundEntities"], roomId, true);

            GetEntity("player").GetComponent<Dog::NativeScriptComponent>().SetVariable("roomBounds", roomBounds);
            GetEntity("player").GetComponent<Dog::NativeScriptComponent>().SetVariable("roomId", int(element["id"]));

            return;
        }
	}
};