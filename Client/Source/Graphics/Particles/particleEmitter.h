#pragma once

#include <PCH/pch.h>
#include <Source/Graphics/ResourceManager/resourceManager.h>

float lerp(float a, float b, float t);

// particle props, filled out with default props
struct ParticleProps {
    glm::vec2 velocity;
    glm::vec2 velocityVariation;
    glm::vec4 color; // Color
    float sizeBegin;
    float sizeEnd;
    float sizeVariation;
    float lifeTime;

    ParticleProps()
        : velocity(0.f, 0.f)
        , velocityVariation(25.f, 25.f)
        , color(1.f)
        , sizeBegin(15.f)
        , sizeEnd(5.f)
        , sizeVariation(8.f)
        , lifeTime(0.75f)
    {
    };

    // Default copy ctor with copies every member over.
    ParticleProps(const ParticleProps& other) = default;
};

struct Particle
{
};

struct ParticleInstanceData {
    glm::vec2 offset;
    glm::vec4 color;
    float scale;
};

static unsigned int ParticlesVAO = 0;
static unsigned int ParticlesInstancedVBO = 0;

class ParticleEmitter
{
public:
    ParticleEmitter(Shader shader, Texture2D texture);
    ParticleEmitter(Shader shader, Texture2D texture, int _poolSize);
    ParticleEmitter(Shader shader, Texture2D texture, int _poolSize, const ParticleProps& props);
    ~ParticleEmitter();

    void OldUpdate(float dt);
    void Update(float dt);
    void GrabParticles(std::vector<ParticleInstanceData>& instanceData);

    void Emit();

    void SetPosition(const glm::vec2& _position);

    ParticleProps particleProps;

private:

    void InitArrays(int poolSize);

    glm::vec2 emitterPosition;

    int poolSize;

    Particle* particlePool;
    bool* activeParticles;

    glm::vec2* position;
    glm::vec2* velocity;
    glm::vec4* color;

    float* rotation;
    float* sizeBegin;
    float* sizeEnd;
    float* lifeTime;
    float* lifeRemaining;

    int poolIndex = 0;

    Shader shader;
    Texture2D texture;
};

void InitParticles(const int maxParticles);
void RenderParticlesInstanced(std::vector<ParticleInstanceData>& instancedData);