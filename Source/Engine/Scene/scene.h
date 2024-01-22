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

		// Scene functions defined by client
		virtual void Init() {}
		virtual void Update(float dt) {}
		virtual void Render(float dt, float ct, float itf) {}
		virtual void Exit() {}

		// Create an entity with the transform and tag components
		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityInGroup(const std::string& group, const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		void DestroyEntityByTag(const std::string& name);
		Entity GetEntityByTag(const std::string& name);
		std::vector<Entity>& GetEntityGroup(const std::string& groupName);
		
	private:
		entt::registry m_Registry;
		std::unordered_map<std::string, Entity> m_TagToEntityMap;
		std::unordered_map<std::string, std::vector<Entity>> m_EntityGroups;
		std::string m_Name;

		b2World* m_PhysicsWorld = nullptr;

		// Scene functions that are the same for every scene
		void InitScene();
		void UpdateScene(float dt);
		void RenderScene(float dt, float ct, float itf);
		void ExitScene();

		friend class Entity;
		friend class SceneManager;
	};

	Entity CreateEntity(const std::string& name = std::string());
	Entity CreateEntityInGroup(const std::string& group, const std::string& name = std::string());

	Entity GetEntity(const std::string& name = std::string());
	std::vector<Entity>& GetEntityGroup(const std::string& groupName);
}