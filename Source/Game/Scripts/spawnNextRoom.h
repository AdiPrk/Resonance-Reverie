#pragma once

#include <Engine/dogpch.h>
#include "playerController.h"
#include "spawnEntities.h"

class SpawnNextRoomScript : public Dog::ScriptableEntity
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

        bool madeNewRoom = false;
        for (const json::value_type& element : data)
        {
            if (element["id"] == GetVariable<int>("currentRoomId")) continue;

            Dog::Rect roomBounds = { element["x"], element["y"], element["w"], element["h"] };
            auto pT = GetEntity("player").GetComponent<Dog::TransformComponent>();
            if (!roomBounds.contains({ pT.Position.x, pT.Position.y, pT.Scale.x, pT.Scale.y })) continue;

            madeNewRoom = true;

            Dog::Renderer::GetCamera().SetMovementBounds(roomBounds);
            int roomId = element["id"];
            printf("createin %i\n", roomId);

            //Dog::Entity bg = CreateEntityInGroup(std::to_string(roomId));
            //bg.AddComponent<Dog::QuadRendererComponent>(glm::vec4(0.2f, 0.2f, 0.2f, 1.f));
            //auto& bgTransform = bg.GetComponent<Dog::TransformComponent>();
            //bgTransform.SetTransform({ roomBounds.left, roomBounds.top }, { roomBounds.width, roomBounds.height });
           
            SpawnEntities(element["backgroundEntities"], roomId, false);
            SpawnEntities(element["foregroundEntities"], roomId, true);

            GetEntity("player").GetComponent<Dog::NativeScriptComponent>().SetVariable("roomBounds", roomBounds);
            GetEntity("player").GetComponent<Dog::NativeScriptComponent>().SetVariable("roomId", int(element["id"]));

            break;
        }

        // Delete old entities
        if (madeNewRoom) {
            int rid = GetVariable<int>("currentRoomId");
            printf("destroy from %i\n", rid);
            auto& oldRoomEntities = GetEntityGroup(std::to_string(rid));
            for (auto& entity : oldRoomEntities)
            {
                DestroyEntity(entity);
            }
            oldRoomEntities.clear();
        }
    }
};