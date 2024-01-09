#include <Engine/dogpch.h>
#include "physics.h"
#include "physicsUtils.h"

namespace Dog {

	static b2BodyType DogRigidbody2DTypeToBox2D(RigidbodyComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case RigidbodyComponent::BodyType::Static: return b2_staticBody;
		case RigidbodyComponent::BodyType::Dynamic: return b2_dynamicBody;
		case RigidbodyComponent::BodyType::Kinematic: return b2_kinematicBody;
		default: throw "Unknown body type";
		}
	}

	void Physics::UpdatePhysicsColliders(entt::registry& m_Registry, b2World* m_PhysicsWorld)
	{
		auto view = m_Registry.view<TransformComponent, RigidbodyComponent, BoxColliderComponent>();
		
		view.each([&](auto entity, TransformComponent& transform, RigidbodyComponent& rigidbody, BoxColliderComponent& boxCollider)
		{
			// If not initializing (after first frame) then remove/add any required fixtures and return
			if (!rigidbody.m_DoInit || !boxCollider.m_DoInit) 
			{
				for (const auto& fixtureName : boxCollider.Box2dFixturesToRemove)
				{
					if (b2Fixture* fix = boxCollider.Box2dFixtures[fixtureName])
					{
						rigidbody.Body->DestroyFixture(fix);
						boxCollider.Box2dFixtures.erase(fixtureName);
					}
				}
				boxCollider.Box2dFixturesToRemove.clear();

				for (const auto& fixtureDef : boxCollider.Box2dFixturesToAdd)
				{
					CreateFixture(fixtureDef, boxCollider, rigidbody);
				}
				boxCollider.Box2dFixturesToAdd.clear();

				return;
			}

			rigidbody.m_DoInit = false;
			boxCollider.m_DoInit = false;
			
			glm::vec2 bodyCenter = transform.Position + transform.Scale * 0.5f;

			b2BodyDef bodyDef;
			bodyDef.type = DogRigidbody2DTypeToBox2D(rigidbody.Type);
			bodyDef.position = Utils::PixelsToMeters(bodyCenter);
			bodyDef.angle = transform.Rotation;
			rigidbody.Body = m_PhysicsWorld->CreateBody(&bodyDef);
			rigidbody.Body->SetGravityScale(rigidbody.GravityScale);
			rigidbody.Body->SetAngularDamping(rigidbody.AngularDamping);
			rigidbody.Body->SetLinearDamping(rigidbody.LinearDamping);

			for (const FixtureDefinition& fixture : boxCollider.Fixtures)
			{
				CreateFixture(fixture, boxCollider, rigidbody);
			}
		});
	}

	void Physics::CreateFixture(const FixtureDefinition& fixture, BoxColliderComponent& boxCollider, RigidbodyComponent& rigidbody)
	{
		b2FixtureDef fixtureDef;
		fixtureDef.isSensor = fixture.IsSensor;

		b2PolygonShape boxShape;
		b2CircleShape circleShape;

		if (fixture.Shape == FixtureDefinition::ShapeType::Box)
		{
			boxShape.SetAsBox(
				Utils::PixelsToMeters(fixture.BoxSize.x * 0.5f),
				Utils::PixelsToMeters(fixture.BoxSize.y * 0.5f),
				Utils::PixelsToMeters(fixture.Offset),
				0.0f
			);
			fixtureDef.shape = &boxShape;
		}
		else if (fixture.Shape == FixtureDefinition::ShapeType::Circle)
		{
			circleShape.m_radius = Utils::PixelsToMeters(fixture.CircleRadius);
			circleShape.m_p = Utils::PixelsToMeters(fixture.Offset);
			fixtureDef.shape = &circleShape;
		}

		fixtureDef.density = fixture.Density;
		fixtureDef.friction = fixture.Friction;
		fixtureDef.restitution = fixture.Restitution;
		// TODO - Add category bits and mask bits
		// fixtureDef.filter.categoryBits = F_BLOCK;
		// fixtureDef.filter.maskBits = 0xFFFF ^ F_LAVA; // Collides with everything except lava
		// fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(entity);

		boxCollider.Box2dFixtures[fixture.Name] = rigidbody.Body->CreateFixture(&fixtureDef);

		boxCollider.Box2dFixturesToAdd.erase(
			std::remove_if(
				boxCollider.Box2dFixturesToAdd.begin(),
				boxCollider.Box2dFixturesToAdd.end(),
				[&fixture](const FixtureDefinition& fd) { return fd.Name == fixture.Name; }
			),
			boxCollider.Box2dFixturesToAdd.end()
		);
	}

	void Physics::RenderColliders(b2World& physicsWorld)
	{
		for (b2Body* body = physicsWorld.GetBodyList(); body != nullptr; body = body->GetNext())
		{
			for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext())
			{
				b2Shape::Type shapeType = fixture->GetType();

				if (shapeType == b2Shape::e_polygon)
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)fixture->GetShape();
					int vertexCount = polygonShape->m_count;

					glm::vec2 centroid = Utils::MetersToPixels(body->GetWorldPoint(polygonShape->m_centroid));

					float minX = FLT_MAX, minY = FLT_MAX, maxX = FLT_MIN, maxY = FLT_MIN;
					for (int i = 0; i < vertexCount; ++i)
					{
						b2Vec2 vertex = body->GetWorldPoint(polygonShape->m_vertices[i]);
						b2Vec2 nextVertex = body->GetWorldPoint(polygonShape->m_vertices[(i + 1) % vertexCount]);
						glm::vec2 p1 = Utils::MetersToPixels(vertex);
						glm::vec2 p2 = Utils::MetersToPixels(nextVertex);

						Renderer::SetShader("defaultsprite");
						Renderer::DrawLine(p1, p2, 2.0f, "square");
					}
				}
				else if (shapeType == b2Shape::e_circle) {
					b2CircleShape* circleShape = (b2CircleShape*)fixture->GetShape();
					glm::vec2 position = Utils::MetersToPixels(body->GetWorldPoint(circleShape->m_p));
					float radius = Utils::MetersToPixels(circleShape->m_radius);
					position -= radius;
					radius *= 2;
					Renderer::DrawSprite("circleOutline2", position, { radius, radius }, 0.0f, { 1.0, 1.0, 1.0, 1.0f });
				}
			}
		}
	}

}