#pragma once

#include <Engine/Physics/physicsUtils.h>
#include "entt.hpp"
class b2World;

namespace Dog {

	struct FixtureDefinition;
	struct BoxColliderComponent;
	struct RigidbodyComponent;

	class Physics {
	public:
		static void RenderColliders(b2World& physicsWorld);
		static void UpdatePhysicsColliders(entt::registry& m_Registry, b2World* m_PhysicsWorld);
		static void CreateFixture(const FixtureDefinition& fixture, BoxColliderComponent& boxCollider, RigidbodyComponent& rigidbody);
		
	};

}