#include <PCH/pch.h>
#include "game.h"

#include <Engine/ResourceManager/resourceManager.h>
#include <Engine/Graphics/Renderer/Camera/camera.h>
#include <Engine/Graphics/Particles/particleEmitter.h>
#include <Engine/Inputs/inputManager.h>
#include <Engine/Audio/audioManager.h>
#include <Engine/Graphics/Renderer/renderer.h>

#include <Game/Entities/Player/player.h>
#include <Game/Physics/physicsWorld.h>

// Game-related State data
ParticleEmitter* playerEmitter;
PhysicsContactListener physicsContactListener;
constexpr int numParticles = 500;
constexpr int backgroundParticles = 10000;
constexpr int totalParticles = numParticles + backgroundParticles;

Game::Game(Window* window)
    : m_State(GAME_ACTIVE)
    , m_RoomBounds(0, 0, 0, 0)
    , m_Player(nullptr)
    , m_Camera(nullptr)
    , m_Width(window->GetWidth())
    , m_Height(window->GetHeight())
    , m_Window(window)
    , backgroundEmitter(nullptr)
    , m_DrawColliders(false)
{
    RandomInit();

    m_Camera = new Camera();

    physicsWorld.SetContactListener(&physicsContactListener);

    Shader::SetupUBO();

    // load textures and shaders
    ResourceManager::LoadTexturesFromDirectory("Game/Assets/Images/");
    ResourceManager::LoadShadersFromDirectory("Game/Assets/Shaders/");

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
    m_Player = new Player(this);
    m_Player->SetupRigidBody();

    // load levels
    GameLevel one; one.LoadStarting("Game/Assets/Maps/map.json", this);

    m_Rooms.push_back(one);

    m_Camera->MoveTo(m_Player->GetPosition(), m_RoomBounds);

    for (int i = 0; i < 100; i++) {
        float bex = m_RoomBounds.left + m_RoomBounds.width * RandomFloat() - 250;
        float bey = m_RoomBounds.top + m_RoomBounds.height * RandomFloat();
        backgroundEmitter->SetPosition({ bex, bey });

        backgroundEmitter->Emit(2);
    }
}

Game::~Game()
{
    delete m_Player;
    delete m_Camera;
    delete playerEmitter;
    delete backgroundEmitter;
}
    
void Game::SetPreviousPositions() {
    m_Player->setPreviousPosition();

    for (auto& room : m_Rooms) {
        for (auto& ent : room.Entities) {
            ent->setPreviousPosition();
        }
    }
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

void Game::Update(float dt)
{
    SetPreviousPositions();
    ResourceManager::GetAudioManager().SetListenerLocation(m_Player->GetPosition());
    ResourceManager::GetAudioManager().Update();

    if (m_FreezeTime > 0) {
        m_FreezeTime -= dt;
        m_SlowdownFactor = 0.0f;

        if (m_FreezeTime <= 0) {
            FilterRooms(dt);
        }

        return;
    }

    if (m_SlowMoTime > 0) {
        m_SlowMoTime -= dt;
    }
    else {
        m_SlowdownFactor = std::lerp(m_SlowdownFactor, 1.0f, 0.15f);
    }

    dt = CalculateSlowedDT(dt);

    // Update entities
    m_Player->Update(dt);
    for (auto& room : m_Rooms) {
        for (auto& ent : room.Entities) {
            ent->Update(dt);
        }
    }

    // Step physics
    m_Player->isGrounded = false;
    physicsWorld.Step(dt, 8, 3);

    // Set updated transforms from physics
    m_Player->SetUpdatedTransform();
    for (auto& room : m_Rooms) {
        for (auto& ent : room.Entities) {
            ent->SetUpdatedTransform();
        }
    }

    if (!m_Player->Bounds().overlaps(m_RoomBounds)) {
        GameLevel newLevel;
        RoomCode loadedNext = newLevel.LoadNext("Game/Assets/Maps/map.json", this, m_Player->Bounds(), true);

        if (loadedNext == ROOM_NOT_FOUND) {
            m_Player->Respawn();
        }
        else if (loadedNext == ROOM_EXISTS) {
            // nothing!
        } 
        else if (loadedNext == ROOM_CREATED) {
            m_Rooms.push_back(newLevel);
            m_FreezeTime = 0.8f;
        }
    }

    Renderer::UpdatePostProcessing(dt);

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

    playerEmitter->Update(dt);
    backgroundEmitter->Update(dt);

    // Wait for threadpool to be done
    ThreadPool::threadPool.wait_for_tasks();
}

void Game::DrawScene(float dt) {
    // draw level backgrounds
    Renderer::SetShader("background");
    for (auto& room : m_Rooms)
    {
        Renderer::DrawSprite(
            "square",
            glm::vec2(room.Bounds().left, room.Bounds().top),
            glm::vec2(room.Bounds().width, room.Bounds().height)
        );
    }

    // draw levels
    int numParticles = playerEmitter->GetParticleCount() + backgroundEmitter->GetParticleCount();
    Renderer::RenderText(std::to_string(numParticles) + " particles", (float)m_Width / 2.f, (float)m_Height / 2.0f - 50, 0.5f, true, glm::vec3(1), true);
    for (auto& room : m_Rooms)
    {
        room.Draw(dt);
    }

    // Particles
    playerEmitter->particleProps.color = { RandomFloat(), RandomFloat(), RandomFloat(), 1.f };
    playerEmitter->GrabParticles();
    backgroundEmitter->GrabParticles();
    playerEmitter->RenderParticlesInstanced();
    backgroundEmitter->RenderParticlesInstanced();

    // draw player
    m_Player->Draw(dt);
}

// delta time, current time, interpolation factor
void Game::Render(float dt, float ct, float itf)
{
    CalculateLerpedPositions(itf);
    UpdateCamera(dt);

    Shader::iTime += CalculateSlowedDT(dt);
    Shader::SetTimeUBO(Shader::iTime);

    // Clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Begin rendering to postprocessing framebuffer
    Renderer::BeginPostProcessing();

    // Draw the scene!
    DrawScene(dt);

    // End rendering to postprocessing framebuffer and render the scene
    Renderer::EndPostProcessing();

    // Draw fps
    std::string fpsString = std::string("fps: ") + std::to_string((1.f / dt));
    Renderer::RenderText(fpsString, 50.f, 90.0f, 0.5f, false, glm::vec3(0));
    Renderer::RenderText(fpsString, 50.f, 50.f, 0.5f, false);

    // fullscreen toggle
    Renderer::RenderText("press F to toggle fullscreen", 50.f, (float)m_Height - 70, 0.5f, false);
    if (InputManager::GetKeyTriggered(Key::F)) {
        m_Window->ToggleFullscreen();
    }

    // draw num particles w/ diagetic text
    
    
    if (InputManager::GetKeyTriggered(Key::K)) {
        m_DrawColliders = !m_DrawColliders;
    }

    if (m_DrawColliders) {
        RenderColliders();
    }
}

void Game::FilterRooms(float dt)
{
    //if (!m_Camera->IsZoomEqualTarget()) return;

    std::vector<GameLevel>& rooms = m_Rooms;
    Rect camRect = this->m_RoomBounds;// m_Camera->Bounds();
    
    // Filter out rooms if out of frame for over a second
    m_Rooms.erase(std::remove_if(m_Rooms.begin(), m_Rooms.end(),
        [&](GameLevel& room) 
        {
            if (!room.Bounds().overlaps(this->Bounds()))
            {    
                for (GameObject* entity : room.Entities) {
                    delete entity;
                }
    
                room.Entities.clear();
    
                return true;
            }
    
            return false;
        }
    ), m_Rooms.end());
}

glm::vec2 Game::GetPlayerPosition()
{
    return m_Player->GetPosition();
}
float Game::GetPlayerRotation()
{
    return m_Player->GetRotation();
}
