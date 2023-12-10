#pragma once

#include "entt.hpp"

class b2World;

namespace Dog {

	class Entity;

	class Scene
	{
	public:
		Scene(const std::string& name);
		~Scene();

		virtual void Init() {}
		virtual void Update(float dt) {}
		virtual void Render(float dt, float ct, float itf) {}
		virtual void Exit() {}

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		void DestroyEntityByTag(const std::string& name);
		Entity GetEntityByTag(const std::string& name);
		
	private:
		entt::registry m_Registry;
		std::unordered_map<std::string, Entity> m_TagToEntityMap;
		std::string m_Name;

		b2World* m_PhysicsWorld = nullptr;

		void InitScene();
		void UpdateScene(float dt);
		void RenderScene(float dt, float ct, float itf);
		void ExitScene();

		friend class Entity;
		friend class SceneManager;
	};

}