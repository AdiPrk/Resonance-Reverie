#include <Engine/PCH/pch.h>

#include <Engine/Graphics/Renderer/renderer.h>
#include <Engine/Physics/physics.h>
#include <Engine/Physics/physicsUtils.h>

#include "scene.h"
#include "Entity/Components/components.h"
#include "Entity/entity.h"
#include "sceneManager.h"

namespace Dog {

	Scene::Scene(const std::string& name)
		: m_Name(name)
	{
	}

	Scene::~Scene()
	{
	}

	// Create an entity with the transform and tag components
	Entity Scene::CreateEntity(const std::string& name)
	{
		// std::cout << "Creating Entity " << name << " in scene " << this->m_Name << std::endl;;
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		std::string nameString = name.empty() ? "Entity" : name;
		tag.Tag = nameString;
		m_TagToEntityMap[name] = entity;
		return entity;
	}

	Entity Scene::CreateEntityInGroup(const std::string& group, const std::string& name)
	{
		auto ent = CreateEntity(name);
		ent.m_Group = group;
		m_EntityGroups[group].push_back(ent);
		return ent;
	}

	Entity CreateEntity(const std::string& name) {
		return SceneManager::GetCurrentScene()->CreateEntity(name);
	}

	Entity CreateEntityInGroup(const std::string& group, const std::string& name) {
		return SceneManager::GetCurrentScene()->CreateEntityInGroup(group, name);
	}

	void Scene::DestroyEntity(Entity entity)
	{
		if (entity.HasComponent<RigidbodyComponent>())
		{
			auto& body = entity.GetComponent<RigidbodyComponent>().Body;
			if (body != nullptr) {
				m_PhysicsWorld->DestroyBody(body);
			}
		}

		m_TagToEntityMap.erase(entity.GetName());
		m_Registry.destroy(entity);
	}

	void Scene::DestroyEntityByTag(const std::string& name)
	{
		m_Registry.destroy(GetEntityByTag(name));
		m_TagToEntityMap.erase(name);
	}

	Entity Scene::GetEntityByTag(const std::string& name)
	{
		return m_TagToEntityMap[name];
	}

	std::vector<Entity>& Scene::GetEntityGroup(const std::string& groupName)
	{
		return m_EntityGroups[groupName];
	}

	Entity GetEntity(const std::string& name) {
		return SceneManager::GetCurrentScene()->GetEntityByTag(name);
	}

	std::vector<Entity>& GetEntityGroup(const std::string& groupName) {
		return SceneManager::GetCurrentScene()->GetEntityGroup(groupName);
	}

	void Scene::InitScene()
	{
		m_PhysicsWorld = new b2World({ 0.0f, 9.81f });
	}

	void Scene::UpdateScene(float dt)
	{
		// Update/create the colliders
		Physics::UpdatePhysicsColliders(m_Registry, m_PhysicsWorld);

		// Run the scripts
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, NativeScriptComponent& nsc)
		{
			if (!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				
				for (auto& [name, value] : nsc.TemporaryVariables) {
					nsc.Instance->SetVariable(name, value);
				}
				nsc.TemporaryVariables.clear();

				nsc.Instance->m_Entity = Entity{ entity, this };
				nsc.Instance->OnCreate();
			}

			nsc.Instance->OnUpdate(dt);
		});

		// Update the physics simulation
		m_PhysicsWorld->Step(dt, 6, 2);

		// Update components with box2d info
		m_Registry.view<TransformComponent, RigidbodyComponent>().each([](auto entity, TransformComponent& transform, RigidbodyComponent& rb)
		{			
			if (!rb.Body) return;
			
			const auto& position = rb.Body->GetPosition();
			transform.Position = Utils::MetersToPixels(position) - transform.Scale * 0.5f;
			transform.Rotation = rb.Body->GetAngle();
		});
	}

	void Scene::RenderScene(float dt, float ct, float itf)
	{
		Renderer::ClearColorAndDepthBuffer();

		m_Registry.view<TransformComponent, QuadRendererComponent>().each([](auto entity, TransformComponent& transform, QuadRendererComponent& quad) {
			Renderer::DrawQuad(transform.Position, transform.Scale, transform.Rotation, quad.Color);
		});

		m_Registry.view<TransformComponent, SpriteRendererComponent>().each([](auto entity, TransformComponent& transform, SpriteRendererComponent& sprite) {
			Renderer::DrawSprite(sprite.Texture, transform.Position, transform.Scale, transform.Rotation, sprite.Color, sprite.Repetition);
		});

		m_Registry.view<TransformComponent, TextRendererComponent>().each([](auto entity, TransformComponent& transform, TextRendererComponent& text) {
			Renderer::RenderText(text.Text, transform.Position.x, transform.Position.y, text.Scale, text.Centered, text.Color, text.Diagetic);
		});

		Physics::RenderColliders(*m_PhysicsWorld);
	}

	void Scene::ExitScene()
	{
		m_Registry.clear();
		m_TagToEntityMap.clear();
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

}