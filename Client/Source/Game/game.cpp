#include <PCH/pch.h>
#include "game.h"

#include <Source/Graphics/Resourcemanager/resourceManager.h>
#include <Source/Graphics/Renderer/spriteRenderer.h>
#include <Source/Graphics/Renderer/textRenderer.h>
#include <Source/Graphics/Renderer/postProcessor.h>
#include <Source/Graphics/Renderer/camera.h>
#include <Source/Graphics/Particles/particleEmitter.h>

#include <Source/Game/Entities/Player/player.h>
#include <Source/Game/Physics/physicsWorld.h>
#include <Source/Game/Inputs/inputManager.h>

#include <Source/Math/rect.h>

// Game-related State data
SpriteRenderer*  Renderer;
TextRenderer*    textRenderer;
ParticleEmitter* playerEmitter;

Game::Game(unsigned int width, unsigned int height)
    : m_State(GAME_ACTIVE)
    , m_CurrentRoom(0)
    , m_RoomBounds(0, 0, 0, 0)
    , m_Player(nullptr)
    , m_Camera(nullptr)
    , m_Width(width)
    , m_Height(height)
    , m_Window(nullptr)
    , Effects(nullptr)
    , backgroundEmitter(nullptr)
    , m_DrawColliders(false)
#if DO_NETWORKING
    , otherPlayers()
#endif
{
}

Game::~Game()
{
    delete Renderer;
    delete m_Player;
    delete m_Camera;
    delete Effects;
    delete textRenderer;
    delete playerEmitter;
    delete backgroundEmitter;
}

constexpr int numParticles = 500;
constexpr int backgroundParticles = 10000;
constexpr int totalParticles = numParticles + backgroundParticles;

PhysicsContactListener physicsContactListener;

void Game::Init(Window* window)
{
    RandomInit();

    m_Window = window;
    m_Camera = new Camera();

    physicsWorld.SetContactListener(&physicsContactListener);

    // load textures
    ResourceManager::LoadTexturesFromDirectory("Assets/Images/");

    // load shaders
    ResourceManager::LoadShadersFromDirectory("Assets/Shaders/");

    // configure shaders
    glm::mat4 projection = glm::ortho(0.f, (float)m_Width, (float)m_Height, 0.f, -1.0f, 1.0f);
    ResourceManager::UpdateAllShaderProjectionMatrices(projection);

    Shader& spriteShader = ResourceManager::GetShader("sprite");
    spriteShader.SetInteger("numLights", 1);
    spriteShader.SetFloat("ambientLightIntensity", 0.3f);

    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), m_Width, m_Height);
    textRenderer = new TextRenderer();
    textRenderer->Load("Assets/Fonts/OCRAEXT.TTF", 60);

    // particles
    playerEmitter = new ParticleEmitter(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), numParticles);
    backgroundEmitter = new ParticleEmitter(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("star"), backgroundParticles);
    InitParticles(totalParticles);

    playerEmitter->particleProps.color = { 0.f, 2.f, 7.f, 1.0f };

    backgroundEmitter->particleProps.lifeTime = 25.0f;
    backgroundEmitter->particleProps.velocity = { 150.0f, 0.0f };
    backgroundEmitter->particleProps.velocityVariation = { 100.f, 30.f };
    backgroundEmitter->particleProps.color = { 1.f, 1.f, 1.f, 1.f };
    backgroundEmitter->particleProps.fadeStyle = PARTICLE_FADE_STYLE::FADE_IN_OUT;
    backgroundEmitter->particleProps.sizeBegin = 25.0f;
    backgroundEmitter->particleProps.sizeEnd = 25.0f;
    backgroundEmitter->particleProps.sizeVariation = 5.0f;
    
    // configure game objects
    m_Player = new Player(ResourceManager::GetTexture("player"), this);
    m_Player->SetupRigidBody();

    // load levels
    GameLevel one; one.LoadStarting("Assets/Maps/map.json", this);

    m_Rooms.push_back(one);
    m_CurrentRoom = 0;

    m_Camera->MoveTo(m_Player->GetPosition(), m_RoomBounds);

    for (int i = 0; i < 100; i++) {
        float bex = m_RoomBounds.left + m_RoomBounds.width * RandomFloat() - 250;
        float bey = m_RoomBounds.top + m_RoomBounds.height * RandomFloat();
        backgroundEmitter->SetPosition({ bex, bey });

        backgroundEmitter->Emit(2);
    }
}
    
void Game::SetPreviousPositions() {
    m_Player->setPreviousPosition();

    for (auto& room : m_Rooms) {
        for (auto& ent : room.Entities) {
            ent->setPreviousPosition();
        }
    }

#if DO_NETWORKING
    for (auto& other : otherPlayers) {
        other.second.oldPos = other.second.currPos;
    }
#endif
}

void Game::CalculateLerpedPositions(float t) {
    m_Player->setLerpedPosition(t);

    for (auto& room : m_Rooms) {
        for (auto& ent : room.Entities) {
            ent->setLerpedPosition(t);
        }
    }
}

float Game::CalculateSlowedDT(float dt) {
    return dt * std::powf(m_SlowdownFactor, 1.5f);
}

void Game::UpdateCamera(float dt)
{
    // Update Camera
    m_Camera->GlideTo(m_Player->GetRenderPosition(), m_RoomBounds, dt);
    m_Camera->UpdateUniforms();
}

void Game::Update(float dt, float accumulator)
{
    if (m_SlowMoTime > 0) {
        m_SlowMoTime -= dt;
    }
    else {
        m_SlowdownFactor = std::lerp(m_SlowdownFactor, 1.0f, 0.15f);
    }
    //m_SlowdownFactor = 0.4f;

    dt = CalculateSlowedDT(dt);

    if (m_State == GAME_ACTIVE)
    {
        m_Player->Update(dt);

        for (auto& room : m_Rooms) {
            const Rect& rb = room.Bounds();

            bool moveEntToNewRoom = false;
            std::vector<GameObject*> entsToMove;
            for (auto& ent : room.Entities) {
                ent->Update(dt);

                if (ent->GetOutOfBounds(rb)) {
                    moveEntToNewRoom = true;
                    entsToMove.push_back(ent);
                }
            }

            if (moveEntToNewRoom) {
                // Remove all entities in entsToMove from this room's list of entities
                room.Entities.erase(std::remove_if(room.Entities.begin(), room.Entities.end(),
                    [&](GameObject* ent) {
                        return std::find(entsToMove.begin(), entsToMove.end(), ent) != entsToMove.end();
                    }), room.Entities.end());

                // Loop through rooms and if the entity is in that room's bounds then add it to that room
                for (auto& entToMove : entsToMove) {
                    for (auto& newRoom : m_Rooms) {
                        if (&newRoom != &room && !entToMove->GetOutOfBounds(newRoom.Bounds())) {
                            newRoom.Entities.push_back(entToMove);
                            break;
                        }
                    }
                }
            }
        }

        m_Player->isGrounded = false;

        physicsWorld.Step(dt, 8, 3);

        m_Player->SetUpdatedPosition();

        for (auto& room : m_Rooms) {
            for (auto& ent : room.Entities) {
                ent->SetUpdatedPosition();
            }
        }
    }

    if (!m_Player->Bounds().overlaps(m_RoomBounds)) {
        GameLevel newLevel;
        RoomCode loadedNext = newLevel.LoadNext("Assets/Maps/map.json", this);

        if (loadedNext == ROOM_NOT_FOUND) {
            m_Player->Respawn();
        }
        else if (loadedNext == ROOM_EXISTS) {
            // nothing!
        } 
        else if (loadedNext == ROOM_CREATED) {
            m_Rooms.push_back(newLevel);
        }      
    }

    Effects->Update(dt);

    // camera->position = player->Position;
    playerEmitter->SetPosition(m_Player->GetRenderPosition() + m_Player->GetSize() / 2.f);

    playerEmitter->particleProps.sizeBegin = 10.0f;
    playerEmitter->particleProps.sizeEnd = 2.0f;
    playerEmitter->particleProps.sizeVariation = 2.0f;

    if (rand() % 2 == 0) {
        playerEmitter->Emit();

        float bex = m_RoomBounds.left - 500.0f;// + m_RoomBounds.width * RandomFloat();
        float bey = m_RoomBounds.top + m_RoomBounds.height * RandomFloat();
        backgroundEmitter->SetPosition({ bex, bey });

        backgroundEmitter->Emit();
    }

   

#if DO_NETWORKING
    for (auto& other : otherPlayers) {
        playerEmitter->SetPosition(other.second.renderPos + m_Player->GetSize() / 2.f);

        for (int i = 0; i < 2; i++) {
            playerEmitter->Emit();
        }
    }
#endif

    playerEmitter->Update(dt);
    backgroundEmitter->Update(dt);

    // Wait for threadpool to be done
    ThreadPool::threadPool.wait_for_tasks();
}

void Game::DrawScene(float t) {
    // draw level backgrounds
    for (auto& room : m_Rooms)
    {
        Renderer->SetShader(ResourceManager::GetShader("background"));
        Renderer->DrawSprite(
            ResourceManager::GetTexture("square"),
            glm::vec3(room.Bounds().left, room.Bounds().top, 0.0f),
            glm::vec2(room.Bounds().width, room.Bounds().height),
            0.0f
        );
    }

    // draw levels
    for (auto& room : m_Rooms)
    {
        room.Draw(*Renderer);
    }

    // Particles
    playerEmitter->particleProps.color = { RandomFloat(), RandomFloat(), RandomFloat(), 1.f };
    playerEmitter->GrabParticles();
    backgroundEmitter->GrabParticles();
    playerEmitter->RenderParticlesInstanced();
    backgroundEmitter->RenderParticlesInstanced();

    // draw player
    m_Player->Draw(*Renderer);

#if DO_NETWORKING
    // draw other players
    for (auto& other : otherPlayers) {

        other.second.renderPos.x = std::lerp(other.second.oldPos.x, other.second.currPos.x, t);
        other.second.renderPos.y = std::lerp(other.second.oldPos.y, other.second.currPos.y, t);

        Renderer->DrawSprite(m_Player->GetSprite(), other.second.renderPos, m_Player->GetSize(), other.second.rotation, m_Player->GetColor());
    }
#endif
}

// delta time, current time, interpolation factor
void Game::Render(float dt, float currentTime, float t)
{
    Shader::iTime += CalculateSlowedDT(dt);
    ResourceManager::UpdateAllShaderTimes(Shader::iTime);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // begin rendering to postprocessing framebuffer
    Effects->BeginRender();

    DrawScene(t);

    // end rendering to postprocessing framebuffer and render postprocessing quad
    Effects->EndRender();
    Effects->Render();

    // draw fps
    std::string fpsString = std::string("fps: ") + std::to_string((1.f / dt));
    textRenderer->RenderText(fpsString, 50.f, 90.0f, 0.5f, false, glm::vec3(0));
    textRenderer->RenderText(fpsString, 50.f, 50.f, 0.5f, false);

    // fullscreen toggle
    textRenderer->RenderText("press F to toggle fullscreen", 50.f, (float)m_Height - 70, 0.5f, false);
    if (InputManager::GetKeyTriggered(Key::F)) {
        m_Window->ToggleFullscreen();
    }

    // show num particles
    int numParticles = playerEmitter->GetParticleCount() + backgroundEmitter->GetParticleCount();
    textRenderer->RenderText(std::to_string(numParticles) + " particles", (float)m_Width / 2.f, (float)m_Height / 2.0f, 0.5f, true, glm::vec3(1), true);
    textRenderer->RenderText(std::to_string(numParticles) + " particles", (float)m_Width / 2.f, (float)m_Height / 2.0f + 50, 0.5f, true, glm::vec3(0), true);

    if (m_State == GAME_MENU)
    {
        //textRenderer->RenderText("Press ENTER to start", this->Width / 2.f, this->Height / 2.0f, 0.5f, true);
    }
    if (m_State == GAME_WIN)
    {
        //textRenderer->RenderText("You!!! You!!!!!", this->Width / 2.f, this->Height / 2.0f - 30.0f, 1.0f, true, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    if (InputManager::GetKeyTriggered(Key::K)) {
        m_DrawColliders = !m_DrawColliders;
    }

    if (m_DrawColliders) {
        RenderColliders(*Renderer);
    }
}

void Game::FilterRooms(float dt)
{
    if (!m_Camera->IsZoomEqualTarget()) return;

    std::vector<GameLevel>& rooms = m_Rooms;
    Rect camRect = m_Camera->Bounds();

    // Filter out rooms if out of frame for over a second
    m_Rooms.erase(std::remove_if(m_Rooms.begin(), m_Rooms.end(),
        [&camRect, &dt](GameLevel& room) 
        {
            if (!room.Bounds().overlaps(camRect))
            {
                room.DeleteTimer() += dt;

                if (room.DeleteTimer() >= 0) {
                    for (GameObject* entity : room.Entities) {
                        delete entity;
                    }

                    room.Entities.clear();

                    return true;
                }
            }
            else 
            {
                room.DeleteTimer() = 0;
            }

            return false;
        }
    ), m_Rooms.end());
}

#if DO_NETWORKING

void Game::PushPlayer(int id)
{
    otherPlayers[id] = OtherPlayer();

    printf("Init player %i\n", id);
}

void Game::PopPlayer(int id)
{
    auto it = otherPlayers.find(id);
    if (it != otherPlayers.end()) {
        otherPlayers.erase(it);

        printf("Remove player %i\n", id);
    }

}

void Game::UpdatePlayer(int id, glm::vec2 pos)
{
    auto it = otherPlayers.find(id);
    if (it != otherPlayers.end()) {
        it->second.currPos = pos;

        //printf("Update player %i at position: (%f, %f)\n", id, pos.x, pos.y);
    }
}

void Game::UpdatePlayerRotation(int id, float rotation)
{
    auto it = otherPlayers.find(id);
    if (it != otherPlayers.end()) {
        it->second.rotation = rotation;

        //printf("Update player %i at position: (%f, %f)\n", id, pos.x, pos.y);
    }
}

glm::vec2 Game::GetPlayerPosition()
{
    return m_Player->GetPosition();
}
float Game::GetPlayerRotation()
{
    return m_Player->GetRotation();
}

#endif