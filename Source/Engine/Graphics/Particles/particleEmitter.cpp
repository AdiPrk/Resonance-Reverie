#include <PCH/pch.h>
#include "particleEmitter.h"
#include <Engine/Math/random.h>

namespace Dog {

	// lerp between two floats
	float lerp(float a, float b, float t) {
		// Ensure t is in range
		t = std::max(0.0f, std::min(1.0f, t));
		return a + t * (b - a);
	}

	void InitParticles(const int maxParticles) {
		// configure VAO/VBO once
		if (ParticlesVAO != 0) return;

		unsigned int VBO;
		float vertices[] = {
			// pos
			0.0f, 1.0f, // Top-left vertex
			0.0f, 0.0f, // Bottom-left vertex
			1.0f, 1.0f, // Top-right vertex
			1.0f, 0.0f  // Bottom-right vertex
		};

		glGenVertexArrays(1, &ParticlesVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		ResourceManager::BindVAO(ParticlesVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

		// Generate a VBO for instance data
		glGenBuffers(1, &ParticlesInstancedVBO);

		glBindBuffer(GL_ARRAY_BUFFER, ParticlesInstancedVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleInstanceData) * maxParticles, nullptr, GL_STREAM_DRAW);

		// Setup the instance attributes
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleInstanceData), (void*)offsetof(ParticleInstanceData, offset));
		glVertexAttribDivisor(1, 1);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleInstanceData), (void*)offsetof(ParticleInstanceData, color));
		glVertexAttribDivisor(2, 1);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleInstanceData), (void*)offsetof(ParticleInstanceData, scale));
		glVertexAttribDivisor(3, 1);
	}

	void ParticleEmitter::InitArrays(int poolSize)
	{
		activeParticles = new bool[poolSize]();

		position = new glm::vec2[poolSize]();
		velocity = new glm::vec2[poolSize]();
		color = new glm::vec4[poolSize]();

		rotation = new float[poolSize]();
		sizeBegin = new float[poolSize]();
		sizeEnd = new float[poolSize]();
		lifeTime = new float[poolSize]();
		lifeRemaining = new float[poolSize]();
		fadeStyle = new PARTICLE_FADE_STYLE[poolSize](FADE_OUT);

		instancedParticleData.reserve(poolSize);
	}

	ParticleEmitter::ParticleEmitter(Shader shader, Texture2D texture)
		: particleProps()
		, shader(shader)
		, texture(texture)
		, poolSize(1000)
		, position()
	{
		InitArrays(poolSize);
	}

	ParticleEmitter::ParticleEmitter(Shader shader, Texture2D texture, int _poolSize)
		: particleProps()
		, shader(shader)
		, texture(texture)
		, poolSize(_poolSize)
		, position()
	{
		InitArrays(poolSize);
	}

	ParticleEmitter::ParticleEmitter(Shader shader, Texture2D texture, int _poolSize, const ParticleProps& props)
		: particleProps(props)
		, shader(shader)
		, texture(texture)
		, poolSize(_poolSize)
		, position()
	{
		InitArrays(poolSize);
	}

	ParticleEmitter::~ParticleEmitter()
	{
		delete[] activeParticles;
		delete[] position;
		delete[] velocity;
		delete[] color;
		delete[] rotation;
		delete[] sizeBegin;
		delete[] sizeEnd;
		delete[] lifeTime;
		delete[] lifeRemaining;
		delete[] fadeStyle;
	}

	// threaded
	void ParticleEmitter::Update(float dt)
	{
		const float rotVel = 0.01f * dt;

		// Use push_loop to parallelize the loop
		ThreadPool::threadPool.push_loop(0, poolSize, [dt, rotVel, this](int start, int end) {
			for (int i = start; i < end; ++i) {
				if (!activeParticles[i]) continue;

				lifeRemaining[i] -= dt;
				position[i] += velocity[i] * dt;
				rotation[i] += rotVel;

				activeParticles[i] = lifeRemaining[i] > 0.0f;
			}
			});
	}

	// unthreaded ver
	void ParticleEmitter::OldUpdate(float dt)
	{
		const float rotVel = 0.01f * dt;

		for (int i = 0; i < poolSize; ++i) {
			if (!activeParticles[i]) continue;

			lifeRemaining[i] -= dt;
			position[i] += velocity[i] * dt;
			rotation[i] += rotVel;

			activeParticles[i] = lifeRemaining[i] > 0.0f;
		}
	}

	void ParticleEmitter::GrabParticles()
	{
		instancedParticleData.clear();

		// Fill the instanceData vector with data from active particles
		for (int i = 0; i < poolSize; i++) {
			if (!activeParticles[i])
				continue;

			float life = lifeRemaining[i] / lifeTime[i];

			switch (fadeStyle[i]) {
			case FADE_OUT:
			{
				color[i].a = life;
				break;
			}
			case FADE_IN_OUT:
			{
				float t = 1 - life;
				color[i].a = t * t * (2 - 2 * t);
				break;
			}
			}

			float size = lerp(sizeEnd[i], sizeBegin[i], life);

			instancedParticleData.emplace_back(position[i] - size * 0.5f, color[i], size);
		}
	}

	void ParticleEmitter::Emit(float emitCount)
	{
		for (int i = 0; i < emitCount; i++) {
			activeParticles[poolIndex] = true;

			position[poolIndex] = emitterPosition;
			rotation[poolIndex] = RandomFloat() * 6.28318531f;

			fadeStyle[poolIndex] = particleProps.fadeStyle;

			velocity[poolIndex] = particleProps.velocity;
			velocity[poolIndex].x += particleProps.velocityVariation.x * (RandomFloat() - 0.5f);
			velocity[poolIndex].y += particleProps.velocityVariation.y * (RandomFloat() - 0.5f);

			color[poolIndex] = particleProps.color;

			lifeTime[poolIndex] = particleProps.lifeTime;
			lifeRemaining[poolIndex] = particleProps.lifeTime;

			sizeBegin[poolIndex] = particleProps.sizeBegin + particleProps.sizeVariation * (RandomFloat() - 0.5f);
			sizeEnd[poolIndex] = particleProps.sizeEnd;

			// Update pool index for next emit
			poolIndex = ++poolIndex % poolSize;
		}
	}

	void ParticleEmitter::SetPosition(const glm::vec2& _position)
	{
		emitterPosition = _position;
	}

	void ParticleEmitter::RenderParticlesInstanced()
	{
		shader.Use();
		shader.SetUniformHandle("textureHandle", texture.textureHandle);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		// Bind the VAO and instance VBO
		ResourceManager::BindVAO(ParticlesVAO);
		glBindBuffer(GL_ARRAY_BUFFER, ParticlesInstancedVBO);

		// Update the instance VBO data
		glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleInstanceData) * instancedParticleData.size(), instancedParticleData.data(), GL_STREAM_DRAW);

		// Draw all particles
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)instancedParticleData.size());

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

}