#pragma once

#include "entity.h"

namespace Dog {

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_Entity.AddComponent<T>(std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}
		template<typename T>
		bool HasComponent()
		{
			return m_Entity.HasComponent<T>();
		}
		Entity GetEntity(const std::string& name) {
			return m_Entity.GetScene()->GetEntityByTag(name);
		}

		void SetVariable(const std::string& name, const std::any& value) {
			m_Variables[name] = value;
		}

		template<typename T>
		T GetVariable(const std::string& name) {
			if (m_Variables.find(name) != m_Variables.end()) {
				return std::any_cast<T>(m_Variables[name]); // No error handling for bad cast
			}
			throw std::runtime_error("Variable not found");
		}

		// Create an entity with the transform and tag components
		Entity CreateEntity(const std::string& name = std::string())
		{
			return m_Entity.GetScene()->CreateEntity(name);
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(float dt) {}

	private:
		friend class Scene;
		
		Entity m_Entity;
		std::unordered_map<std::string, std::any> m_Variables;

	};

}
