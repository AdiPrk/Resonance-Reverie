#pragma once

#include <Engine/PCH/pch.h>
#include <Engine/ResourceManager/resourceManager.h>
#include <Engine/Math/utils.h>

namespace Dog {

    enum PARTICLE_FADE_STYLE {
        NONE = 0,
        FADE_OUT = 1,
        FADE_IN_OUT = 2
    };

    // particle props, filled out with default props
    struct ParticleProps {
        glm::vec2 velocity;
        glm::vec2 velocityVariation;
        glm::vec4 color; // Color
        float sizeBegin;
        float sizeEnd;
        float sizeVariation;
        float lifeTime;
        PARTICLE_FADE_STYLE fadeStyle;
        float angle;
        float angularVelocity;

        ParticleProps()
            : velocity(0.f, 0.f)
            , velocityVariation(25.f, 25.f)
            , color(1.f)
            , sizeBegin(15.f)
            , sizeEnd(5.f)
            , sizeVariation(2.f)
            , lifeTime(1.0f)
            , fadeStyle(PARTICLE_FADE_STYLE::FADE_OUT)
            , angle(0.0f)
            , angularVelocity(0.0f)
        {
        };

        // Default copy ctor with copies every member over.
        ParticleProps(const ParticleProps& other) = default;
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
        void GrabParticles();

        void Emit(float emitCount = 1);

        void SetPosition(const glm::vec2& _position);
        const glm::vec2 GetPosition() const { return emitterPosition; }
        void RenderParticlesInstanced();

        ParticleProps particleProps;

        int GetParticleCount() { return static_cast<int>(instancedParticleData.size()); }

    private:

        void InitArrays(int poolSize);

        glm::vec2 emitterPosition;

        int poolSize;

        bool* activeParticles;

        glm::vec2* position;
        glm::vec2* velocity;
        glm::vec4* color;

        float* rotation;
        float* sizeBegin;
        float* sizeEnd;
        float* lifeTime;
        float* lifeRemaining;

        PARTICLE_FADE_STYLE* fadeStyle;

        int poolIndex = 0;

        Shader shader;
        Texture2D texture;

        std::vector<ParticleInstanceData> instancedParticleData;
    };

    void InitParticles(const int maxParticles);

}