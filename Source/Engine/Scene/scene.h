#pragma once

#include "entt.hpp"

class Entity;

class Scene
{
public:
	Scene();
	~Scene();

	Entity CreateEntity(const std::string& name = std::string());

	void OnUpdate(float dt);

private:
	entt::registry m_Registry;

	friend class Entity;
};