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

    // load shaders
    ResourceManager::LoadShader("Assets/Shaders/sprite.vert", "Assets/Shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("Assets/Shaders/sprite.vert", "Assets/Shaders/spritetile.frag", nullptr, "spritetiles");
    ResourceManager::LoadShader("Assets/Shaders/sprite.vert", "Assets/Shaders/spriteflagtiles.frag", nullptr, "spriteflagtiles");
    ResourceManager::LoadShader("Assets/Shaders/sprite.vert", "Assets/Shaders/spriteillbeback.frag", nullptr, "spriteillbeback");
    ResourceManager::LoadShader("Assets/Shaders/sprite.vert", "Assets/Shaders/spritelava.frag", nullptr, "spritelava");
    ResourceManager::LoadShader("Assets/Shaders/sprite.vert", "Assets/Shaders/spritesaber.frag", nullptr, "spritesaber");
    ResourceManager::LoadShader("Assets/Shaders/particle.vert", "Assets/Shaders/particle.frag", nullptr, "particle");
    ResourceManager::LoadShader("Assets/Shaders/postprocess.vert", "Assets/Shaders/postprocess.frag", nullptr, "postprocessing");
    ResourceManager::LoadShader("Assets/Shaders/text.vert", "Assets/Shaders/text.frag", nullptr, "text");

    // configure shaders
    glm::mat4 projection = glm::ortho(0.f, (float)m_Width, (float)m_Height, 0.f, -1.0f, 1.0f);

    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("sprite").SetFloat("iTime", 0);
    ResourceManager::GetShader("sprite").SetVector2f("repetition", 0, 0);

    ResourceManager::GetShader("spritetiles").Use().SetInteger("image", 0);
    ResourceManager::GetShader("spritetiles").SetMatrix4("projection", projection);
    ResourceManager::GetShader("spritetiles").SetFloat("iTime", 0);

    ResourceManager::GetShader("spriteflagtiles").Use().SetInteger("image", 0);
    ResourceManager::GetShader("spriteflagtiles").SetMatrix4("projection", projection);
    ResourceManager::GetShader("spriteflagtiles").SetFloat("iTime", 0);

    ResourceManager::GetShader("spriteillbeback").Use().SetInteger("image", 0);
    ResourceManager::GetShader("spriteillbeback").SetMatrix4("projection", projection);
    ResourceManager::GetShader("spriteillbeback").SetFloat("iTime", 0);

    ResourceManager::GetShader("spritelava").Use().SetInteger("image", 0);
    ResourceManager::GetShader("spritelava").SetMatrix4("projection", projection);
    ResourceManager::GetShader("spritelava").SetFloat("iTime", 0);

    ResourceManager::GetShader("spritesaber").Use().SetInteger("image", 0);
    ResourceManager::GetShader("spritesaber").SetMatrix4("projection", projection);
    ResourceManager::GetShader("spritesaber").SetFloat("iTime", 0);

    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projectionView", projection);

    ResourceManager::GetShader("text").Use().SetInteger("text", 0);
    ResourceManager::GetShader("text").SetMatrix4("projection", projection);

    // load textures
    ResourceManager::LoadTexture("Assets/Images/awesomeface.png", "face");
    ResourceManager::LoadTexture("Assets/Images/block.png", "block");
    ResourceManager::LoadTexture("Assets/Images/block_solid.png", "block_solid");
    ResourceManager::LoadTexture("Assets/Images/particle.png", "particle");
    ResourceManager::LoadTexture("Assets/Images/particle2.png", "particle2");
    ResourceManager::LoadTexture("Assets/Images/square.png", "square");
    ResourceManager::LoadTexture("Assets/Images/circle.png", "circle");
    ResourceManager::LoadTexture("Assets/Images/player.png", "player");
    ResourceManager::LoadTexture("Assets/Images/circleOutline.png", "circleOutline");

    //ResourceManager::GetTexture("block_solid")

    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), m_Width, m_Height);
    textRenderer = new TextRenderer();
    textRenderer->Load("Assets/Fonts/OCRAEXT.TTF", 60);
    
    // particles
    playerEmitter = new ParticleEmitter(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), numParticles);
    backgroundEmitter = new ParticleEmitter(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("circle"), backgroundParticles);
    InitParticles(totalParticles);

    playerEmitter->particleProps.color = { 0.f, 2.f, 7.f, 1.0f };
    playerEmitter->particleProps.velocityVariation = { 20.f, 20.f };
    playerEmitter->particleProps.sizeVariation = 1.0f;
    playerEmitter->particleProps.lifeTime = 1.0f;
    playerEmitter->particleProps.fadeStyle = PARTICLE_FADE_STYLE::FADE_OUT;

    backgroundEmitter->particleProps.lifeTime = 25.0f;
    backgroundEmitter->particleProps.velocity = { 150.0f, 0.0f };
    backgroundEmitter->particleProps.velocityVariation = { 100.f, 30.f };
    backgroundEmitter->particleProps.color = { 1.f, 1.f, 1.f, 1.f };
    backgroundEmitter->particleProps.fadeStyle = PARTICLE_FADE_STYLE::FADE_IN_OUT;
    
    // configure game objects
    m_Player = new Player(ResourceManager::GetTexture("player"), this);

    // load levels
    GameLevel one; one.LoadStarting("Assets/Maps/map.json", this);

    m_Rooms.push_back(one);
    m_CurrentRoom = 0;

    m_Player->SetupRigidBody();

    m_Camera->CalculateBoundingRect();
    m_Camera->MoveTo(m_Player->GetPosition(), m_RoomBounds);
    m_Camera->UpdateUniforms();

    for (int i = 0; i < 100; i++) {
        float bex = m_RoomBounds.left + m_RoomBounds.width * RandomFloat() - 500.0f;
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
    this->ProcessInput(dt);

    if (m_SlowMoTime > 0) {
        m_SlowMoTime -= dt;
    }
    else {
        m_SlowdownFactor = std::lerp(m_SlowdownFactor, 1.0f, 0.15f);
    }

    dt = CalculateSlowedDT(dt);

    if (m_State == GAME_ACTIVE)
    {
        m_Player->Update(dt);

        for (auto& room : m_Rooms) {
            for (auto& ent : room.Entities) {
                if (ent->active) {
                    ent->Update(dt);
                    ent->UpdateOutOfBounds(room.Bounds());
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

        backgroundEmitter->Emit(2);
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

// delta time, current time, interpolation factor
void Game::Render(float dt, float currentTime, float t)
{
    Shader::iTime += CalculateSlowedDT(dt);
    ResourceManager::GetShader("sprite").SetFloat("iTime", Shader::iTime, true);
    ResourceManager::GetShader("spritetiles").SetFloat("iTime", Shader::iTime, true);
    ResourceManager::GetShader("spriteflagtiles").SetFloat("iTime", Shader::iTime, true);
    ResourceManager::GetShader("spritelava").SetFloat("iTime", Shader::iTime, true);
    ResourceManager::GetShader("spriteillbeback").SetFloat("iTime", Shader::iTime, true);
    ResourceManager::GetShader("spritesaber").SetFloat("iTime", Shader::iTime, true);

    playerEmitter->particleProps.color = { RandomFloat(), RandomFloat(), RandomFloat(), 1.f};

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_State == GAME_ACTIVE || m_State == GAME_MENU || m_State == GAME_WIN)
    {
        // begin rendering to postprocessing framebuffer
        Effects->BeginRender();

            // draw background

            // draw level
            for (auto& room : m_Rooms) {

                Renderer->SetShader(ResourceManager::GetShader("sprite"));
                Renderer->DrawSprite(
                    ResourceManager::GetTexture("square"),
                    glm::vec3(room.Bounds().left, room.Bounds().top, 0.0f),
                    glm::vec2(room.Bounds().width, room.Bounds().height),
                    0.0f,
                    { 0.0f, 0.0f, 0.0f, 1.0f }
                );

                room.Draw(*Renderer);
            }

            // Particles
            playerEmitter->GrabParticles();
            backgroundEmitter->GrabParticles();
            playerEmitter->RenderParticlesInstanced();
            backgroundEmitter->RenderParticlesInstanced();
            
            // draw player
            m_Player->Draw(*Renderer);

            // draw other players
            Renderer->SetShader(ResourceManager::GetShader("sprite"));

#if DO_NETWORKING
            for (auto& other : otherPlayers) {

                other.second.renderPos.x = std::lerp(other.second.oldPos.x, other.second.currPos.x, t);
                other.second.renderPos.y = std::lerp(other.second.oldPos.y, other.second.currPos.y, t);

                Renderer->DrawSprite(m_Player->GetSprite(), other.second.renderPos, m_Player->GetSize(), m_Player->GetRotation(), m_Player->GetColor());
            }
#endif

        // end rendering to postprocessing framebuffer and render postprocessing quad
        Effects->EndRender();
        Effects->Render((float)glfwGetTime());

        // render text
        std::string fpsString = std::string("fps: ") + std::to_string((1.f / dt));
        textRenderer->RenderText(fpsString, 50.f, 90.0f, 0.5f, false, glm::vec3(0));
        textRenderer->RenderText(fpsString, 50.f, 50.f, 0.5f, false);
        
        textRenderer->RenderText("press F to toggle fullscreen", 50.f, (float)m_Height - 70, 0.5f, false);
        
        if (InputManager::GetKeyDownOnce(GLFW_KEY_F)) {
            m_Window->ToggleFullscreen();
        }

        int numParticles = playerEmitter->GetParticleCount() + backgroundEmitter->GetParticleCount();
        textRenderer->RenderText(std::to_string(numParticles) + " particles", (float)m_Width / 2.f, (float)m_Height / 2.0f, 0.5f, true, glm::vec3(1), true);
    }
    if (m_State == GAME_MENU)
    {
        //textRenderer->RenderText("Press ENTER to start", this->Width / 2.f, this->Height / 2.0f, 0.5f, true);
    }
    if (m_State == GAME_WIN)
    {
        //textRenderer->RenderText("You!!! You!!!!!", this->Width / 2.f, this->Height / 2.0f - 30.0f, 1.0f, true, glm::vec3(0.0f, 1.0f, 0.0f));
    }

#if DEBUG_DRAW_COLLIDERS
    this->RenderColliders();
#endif
}

void Game::RenderColliders()
{
    for (b2Body* body = physicsWorld.GetBodyList(); body != nullptr; body = body->GetNext())
    {
        float angle = body->GetAngle();

        // Check if the body is the anchor
        if (body->GetType() == b2_staticBody && body->GetFixtureList() == nullptr)
        {
            glm::vec2 anchorPosition = PhysicsUtils::MetersToPixels(body->GetPosition());
            float anchorSize = 5.0f;

            Renderer->DrawSprite(ResourceManager::GetTexture("particle"), anchorPosition, glm::vec2(anchorSize), 0, { 1.0, 0.0, 0.0, 1.0f });
            continue;
        }

        for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext())
        {
            b2Shape::Type shapeType = fixture->GetType();

            if (shapeType == b2Shape::e_polygon)
            {
                b2PolygonShape* polygonShape = (b2PolygonShape*)fixture->GetShape();
                int vertexCount = polygonShape->m_count;

                glm::vec2 centroid = PhysicsUtils::MetersToPixels(body->GetWorldPoint(polygonShape->m_centroid));

                float minX = FLT_MAX, minY = FLT_MAX, maxX = FLT_MIN, maxY = FLT_MIN;
                for (int i = 0; i < vertexCount; ++i)
                {
                    b2Vec2 vertex = polygonShape->m_vertices[i];
                    minX = std::min(minX, vertex.x);
                    minY = std::min(minY, vertex.y);
                    maxX = std::max(maxX, vertex.x);
                    maxY = std::max(maxY, vertex.y);
                }

                glm::vec2 size = PhysicsUtils::MetersToPixels(b2Vec2(maxX - minX, maxY - minY));
                glm::vec2 position = centroid - size / 2.f;

                uint16 bits = fixture->GetFilterData().categoryBits;

                // drawing particle cuz it has just an outline lol
                if (bits == F_PLAYER) {
                    Renderer->DrawSprite(ResourceManager::GetTexture("particle"), position, size, angle, COLOR_F_PLAYER);
                }
                else if (bits == F_BLOCK) {
                    Renderer->DrawSprite(ResourceManager::GetTexture("particle"), position, size, angle, COLOR_F_BLOCK);
                }
                else if (bits == F_LAVA) {
                    Renderer->DrawSprite(ResourceManager::GetTexture("particle"), position, size, angle, COLOR_F_LAVA);
                }
                else {
                    // default
                    Renderer->DrawSprite(ResourceManager::GetTexture("square"), position, size, angle, { 1.0, 1.0, 1.0, 1.0f });
                }
            }
            else if (shapeType == b2Shape::e_circle) {
                b2CircleShape* circleShape = (b2CircleShape*)fixture->GetShape();
                glm::vec2 position = PhysicsUtils::MetersToPixels(body->GetWorldPoint(circleShape->m_p));
                float radius = PhysicsUtils::MetersToPixels(circleShape->m_radius);
                position -= radius;
                radius *= 2;
                Renderer->DrawSprite(ResourceManager::GetTexture("circleOutline"), position, { radius, radius }, angle, { 1.0, 1.0, 1.0, 1.0f });
            }
            // else, handle other types in the future eg shapeType == b2Shape::e_circle
        }
    }
}

void Game::ProcessInput(float dt)
{
    if (m_State == GAME_MENU)
    {
        
    }
    if (m_State == GAME_ACTIVE)
    {

    }
    if (m_State == GAME_WIN)
    {
        
    }
}

void Game::ResetLevel()
{
    //if (this->Level == 0)
    //this->Rooms[0].Load("Assets/Maps/map.json", this);
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

                if (room.DeleteTimer() > 1) {
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

glm::vec2 Game::GetPlayerPosition()
{
    return m_Player->GetPosition();
}

#endif