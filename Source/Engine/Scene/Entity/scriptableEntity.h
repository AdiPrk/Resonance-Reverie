#pragma once

#include "entity.h"

namespace Dog {

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}
		Entity GetEntity(const std::string& name) {
			return m_Entity.GetScene()->GetEntityByTag(name);
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(float dt) {}

	private:
		Entity m_Entity;
		friend class Scene;
	};

}
