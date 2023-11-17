#include <PCH/pch.h>
#include <Source/Graphics/ResourceManager/resourceManager.h>

#include <Source/Game/Entities/gameObject.h>
#include <Source/Game/Entities/Player/player.h>
#include <Source/Game/Entities/Blocks/block.h>
#include <Source/Game/Entities/Blocks/dynamicBlock.h>
#include <Source/Game/Entities/Blocks/swingingBlock.h>
#include <Source/Game/Entities/Blocks/lava.h>
#include <Source/Game/Entities/Blocks/safeZone.h>
#include <Source/Game/Entities/Enemies/greyEnemy.h>
#include <Source/Game/Entities/Interactables/grapplePoint.h>
#include <Source/Game/Entities/Environment/light.h>

#include <Source/Graphics/Renderer/spriteRenderer.h>
#include <Source/Graphics/Renderer/camera.h>

#include <Source/Game/game.h>
#include "gameLevel.h"

const float gridSize = 25.0f;

GameLevel::~GameLevel()
{
}

void GameLevel::SetupRoom(Game* game, auto element, bool starting, bool isCurrent, bool setAsCurrent) {
    Rect roomBounds(element["x"], element["y"], element["w"], element["h"]);

    // std::cout << "Creating Room: " << element << std::endl;

    if (isCurrent && setAsCurrent) {
        float camScale = element["camScale"];
        SetCurrentRoomInfo(game, roomBounds, camScale, element);
    }

    m_BoundingRect = roomBounds;
    m_ID = element["id"];

    // Accessing array inside the JSON
    float minDistToSpawn = FLT_MAX;

    for (const auto& entity : element["entities"]) {
        // std::cout << "Entity: " << entity << std::endl;

        glm::vec2 pos(entity["x"], entity["y"]);
        glm::vec2 size(entity["w"], entity["h"]);

        if (entity["type"] == "1" && game->m_Player->m_initPosition) // player init position
        {
            // set player pos
            Player* player = game->m_Player;
            player->m_initPosition = false;
            player->m_Position = pos;
            player->m_SpawnPosition = pos;
            player->SetupRigidBody();
        }
        else if (entity["type"] == "2" && !starting && isCurrent) // player spawn position
        {
            // set player pos
            if (glm::distance(pos, game->m_Player->m_Position) < minDistToSpawn) {
                game->m_Player->m_SpawnPosition = pos;
                minDistToSpawn = glm::distance(pos, game->m_Player->m_Position);
            }
        }
        else if (entity["type"] == "3") // solid
        {
            float rotation = entity["rotation"];

            if (entity["dynamic"] == false) {
                Block* obj = new Block(pos, size, rotation, ResourceManager::GetTexture("block_solid"));
                obj->SetupRigidBody();
                this->Entities.push_back(obj);
            }
            else{
                DynamicBlock* obj = new DynamicBlock(pos, size, rotation, ResourceManager::GetTexture("block_solid"));
                obj->SetRestitution(entity["restitution"]);
                obj->SetDensity(entity["density"]);
                obj->SetupRigidBody();
                this->Entities.push_back(obj);
            }
        }
        else if (entity["type"] == "4") // lava 
        {
            Lava* obj = new Lava(pos, size, ResourceManager::GetTexture("square"));
            obj->SetupRigidBody();
            this->Entities.push_back(obj);
        }
        else if (entity["type"] == "5") // safe zone
        {
            SafeZone* obj = new SafeZone(pos, size, ResourceManager::GetTexture("square"));
            obj->SetupRigidBody();
            this->Entities.push_back(obj);
        }
        else if (entity["type"] == "11") // light
        {
            Light* obj = new Light(pos, entity["radius"] * gridSize, entity["intensity"]);
            this->Entities.push_back(obj);
        }
        else if (entity["type"] == "14") { // Enemy spawner
            float numGreys = entity["numGreys"];
            Rect elementBounds = { pos.x, pos.y, size.x, size.y };
            SpawnEnemies(0, (int)numGreys, elementBounds);
        }
        else if (entity["type"] == "15") // safe zone
        {
            float rotation = entity["rotation"];

            SwingingBlock* obj = new SwingingBlock(pos, size, rotation, ResourceManager::GetTexture("block"));
            obj->anchorPos = glm::vec2(entity["anchorX"], entity["anchorY"]);
            obj->lineWidth = entity["lineWidth"];
            obj->SetDensity(entity["density"]);
            obj->SetupRigidBody();
            this->Entities.push_back(obj);
        }
        else if (entity["type"] == "16") { // Enemy spawner
            GrapplePoint* obj = new GrapplePoint(pos, size, entity["radius"] * gridSize, ResourceManager::GetTexture("circle"));
            obj->SetupRigidBody();
            this->Entities.push_back(obj);
        }
    }

    std::sort(Entities.begin(), Entities.end(), [](GameObject* a, GameObject* b) {
        return a->GetRenderDepth() < b->GetRenderDepth();
    });
}

void GameLevel::SetCurrentRoomInfo(Game* game, Rect& roomBounds, float camScale, auto element)
{
    game->SetBounds(roomBounds);
    game->m_Camera->SetScale(camScale);

    float minDistToSpawn = FLT_MAX;

    for (const auto& entity : element["entities"]) {
        if (entity["type"] == "2") // player spawn position
        {
            // set player pos
            glm::vec2 pos(entity["x"], entity["y"]);
            glm::vec2 size(entity["w"], entity["h"]);

            if (glm::distance(pos, game->m_Player->m_Position) < minDistToSpawn) {
                game->m_Player->m_SpawnPosition = pos;
                minDistToSpawn = glm::distance(pos, game->m_Player->m_Position);
            }
        }
    }
}

void GameLevel::LoadStarting(const char* filename, Game* game)
{
    // Open the file
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Error opening file: " << filename << std::endl;
        return;
    }

    // Parse the file and load into a json object
    json data = json::parse(file);

    // loop over rooms
    for (const json::value_type& element : data) 
    {
        if (!element.contains("starting")) continue;
        if (element["starting"] != true) continue;

        SetupRoom(game, element, true, true, true);

        return;
    }
}

RoomCode GameLevel::LoadNext(const char* filename, Game* game, const Rect& boundsToSearch, bool setAsCurrent)
{
    // Open the file
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Error opening file: " << filename << std::endl;
        return ROOM_NOT_FOUND;
    }

    // Parse the file and load into a json object
    json data = json::parse(file);

    // Loop over rooms
    RoomCode roomCode = ROOM_NOT_FOUND;

    for (const auto& element : data) {
        Rect roomBounds = { element["x"], element["y"], element["w"], element["h"] };

        if (!roomBounds.overlaps(boundsToSearch)) continue;

        for (const auto& room : game->m_Rooms)
        {
            if (room.m_ID != element["id"]) continue;
            //if (room.Bounds().contains(game->GetPlayerPosition())) continue;

            if (setAsCurrent) {
                float camScale = element["camScale"];
                SetCurrentRoomInfo(game, roomBounds, camScale, element);
            }

            return ROOM_EXISTS;
        }

        roomCode = ROOM_CREATED;

        // Main new room:
        if (setAsCurrent) {
            SetupRoom(game, element, false, true, setAsCurrent);
        }
        else {
            SetupRoom(game, element, false, false, setAsCurrent);
        }
        if (setAsCurrent == false) {
            return roomCode;
        }

        // Nearby rooms that are temporarily loaded to make it look nicer when 
        // camera is switching between rooms
        for (const auto& other : data) {
            // same room
            if (element["id"] == other["id"]) continue;

            // room already exists?
            bool roomExists = false;
            for (const auto& room : game->m_Rooms) {
                if (room.m_ID != other["id"]) continue;

                roomExists = true;
                break;
            }

            if (roomExists) continue;

            Rect otherRoomBounds = { other["x"], other["y"], other["w"], other["h"] };

            if (roomBounds.bordersOverlap(otherRoomBounds)) {
                // Setup the room! :)
                GameLevel newLevel;
                newLevel.SetupRoom(game, other, false, false, false);
                game->m_Rooms.push_back(newLevel);
            }
        }
    }

    return roomCode;
}

void GameLevel::Draw(SpriteRenderer& renderer)
{
    Light::lightIndex = 0;

    for (GameObject*& entity : this->Entities) {
        entity->Draw(renderer);

        entity->SetLightInfo();
    }
}

void GameLevel::SpawnEnemies(int enemyID, int numEnemies, Rect spawnBounds)
{
    for (int i = 0; i < numEnemies; i++)
    {
        glm::vec2 size = { gridSize, gridSize };

        glm::vec2 pos = {
            spawnBounds.left + size.x + RandomFloat() * (spawnBounds.width - size.x * 2),
            spawnBounds.top + size.y + RandomFloat() * (spawnBounds.height - size.y * 2)
        };

        GreyEnemy* obj = new GreyEnemy(pos, size, ResourceManager::GetTexture("circle"), glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
        obj->SetupRigidBody();
        this->Entities.push_back(obj);
    }
}