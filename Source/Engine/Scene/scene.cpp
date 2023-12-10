#include <Engine/PCH/pch.h>
#include "scene.h"

#include "Entity/Components/components.h"
#include <Engine/Graphics/Renderer/renderer.h>

#include "Entity/entity.h"

namespace Dog {

	Scene::Scene(const std::string& name)
		: m_Name(name)
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		std::string nameString = name.empty() ? "Entity" : name;
		tag.Tag = nameString;
		m_TagToEntityMap[name] = entity;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
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

	void Scene::InitScene()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.81f });
	}

	void Scene::UpdateScene(float dt)
	{
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
		{
			if (!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = Entity{ entity, this };
				nsc.Instance->OnCreate();
			}

			nsc.Instance->OnUpdate(dt);
		});
	}

	void Scene::RenderScene(float dt, float ct, float itf)
	{
		Renderer::ClearColorAndDepthBuffer();

		m_Registry.view<TransformComponent, QuadRendererComponent>().each([](auto entity, TransformComponent& transform, QuadRendererComponent& quad) {
			Renderer::DrawQuad(transform.position, transform.scale, transform.rotation, quad.color);
		});

		m_Registry.view<TransformComponent, SpriteRendererComponent>().each([](auto entity, TransformComponent& transform, SpriteRendererComponent& sprite) {
			Renderer::DrawSprite(sprite.texture, transform.position, transform.scale, transform.rotation, sprite.color);
		});

		m_Registry.view<TransformComponent, TextRendererComponent>().each([](auto entity, TransformComponent& transform, TextRendererComponent& text) {
			Renderer::RenderText(text.text, transform.position.x, transform.position.y, text.scale, text.centered, text.color, text.diagetic);
		});
	}

	void Scene::ExitScene()
	{
		m_Registry.clear();
		m_TagToEntityMap.clear();
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

}