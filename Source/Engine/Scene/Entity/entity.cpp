#include <Engine/PCH/pch.h>
#include "entity.h"

namespace Dog {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

}