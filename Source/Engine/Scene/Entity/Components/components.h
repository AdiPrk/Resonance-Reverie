#pragma once

#include <Engine/PCH/pch.h>
#include <Engine/Graphics/Texture/texture.h>
#include <Engine/ResourceManager/resourceManager.h>
#include <Engine/Physics/physics.h>

namespace Dog {

	// Forward Declarations ----------- //
	class ScriptableEntity;

	// -------------------------------- //
	// Basic Components --------------- //

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec2 Position = { 0.0f, 0.0f };
		glm::vec2 Scale = { 50.0f, 50.0f };
		float Rotation = 0;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec2& pos, const glm::vec2& size, float rot)
			: Position(pos)
			, Scale(size)
			, Rotation(rot) {}

		void SetTransform(const glm::vec2& pos, const glm::vec2& size, float rot) {
			Position = pos;
			Scale = size;
			Rotation = rot;
		}
		void SetTransform(const glm::vec2& pos, const glm::vec2& size) {
			SetTransform(pos, size, Rotation);
		}
		void SetTransform(const glm::vec2& pos) {
			SetTransform(pos, Scale, Rotation);
		}
		void SetTransform(const TransformComponent& transform) {
			Position = transform.Position;
			Scale = transform.Scale;
			Rotation = transform.Rotation;
		}
	};

	// -------------------------------- //
	// Graphics Components ------------ //

	struct QuadRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		QuadRendererComponent() = default;
		QuadRendererComponent(const QuadRendererComponent&) = default;
		QuadRendererComponent(const glm::vec4& col)
			: Color(col) {}
	};

	struct SpriteRendererComponent
	{
		Texture2D Texture;
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec2 Repetition{ 0.0f, 0.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const std::string& texName, const glm::vec4& col = { 1.0f, 1.0f, 1.0f, 1.0f }, const glm::vec2& repetition = { 0.0f, 0.0f })
			: Texture(ResourceManager::GetTexture(texName))
			, Color(col)
			, Repetition(repetition) {}
		SpriteRendererComponent(const Texture2D& tex, const glm::vec4& col = { 1.0f, 1.0f, 1.0f, 1.0f }, const glm::vec2& repetition = { 0.0f, 0.0f })
			: Texture(tex)
			, Color(col) 
			, Repetition(repetition) {}
	};

	struct TextRendererComponent
	{
		std::string Text{};

		// The text scale. The default scale is one.
		float Scale = 1;
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		bool Centered = true;
		bool Diagetic = true;

		TextRendererComponent() = default;
		TextRendererComponent(const TextRendererComponent&) = default;
		TextRendererComponent(const std::string& text, float size = 1, const glm::vec4& col = { 1.0f, 1.0f, 1.0f, 1.0f }, bool alignCenter = true, bool useCamera = true)
			: Text(text)
			, Scale(size)
			, Color(col)
			, Centered(alignCenter)
			, Diagetic(useCamera) {}
	};

	// -------------------------------- //
	// Physics Components ------------- //

	struct RigidbodyComponent
	{
		enum class BodyType { Static, Dynamic, Kinematic };

		BodyType Type = BodyType::Static;
		bool FixedRotation = false;
		float LinearDamping = 0.0f;
		float AngularDamping = 0.0f;
		float GravityScale = 1.0f;

		b2Body* Body = nullptr;

		RigidbodyComponent() = default;
		RigidbodyComponent(const RigidbodyComponent&) = default;
		RigidbodyComponent(BodyType type, bool fixedRotation = false, float linearDamping = 0.0f, float angularDamping = 0.0f, float gravityScale = 1.0f)
			: Type(type)
			, FixedRotation(fixedRotation)
			, LinearDamping(linearDamping)
			, AngularDamping(angularDamping)
			, GravityScale(gravityScale) {}

	private:
		friend Scene;
		friend Physics;
		bool m_DoInit = true;
	};

	struct FixtureDefinition {
		enum class ShapeType { Box, Circle };
		ShapeType Shape = ShapeType::Box;

		glm::vec2 Offset = { 0.0f, 0.0f };
		
		// Shape properties
		glm::vec2 BoxSize = { 1.0f, 1.0f };
		float CircleRadius = 1.0f;
		
		// Common fixture properties
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		bool IsSensor = false;

		std::string Name = "";

		// Constructor and other methods as needed
		FixtureDefinition() = default;
		FixtureDefinition(const FixtureDefinition&) = default;
		FixtureDefinition(ShapeType shapeType, const glm::vec2& offset = { 0.0f, 0.0f }, const glm::vec2& boxSize = { 1.0f, 1.0f }
						, float density = 1.0f, float friction = 0.5f, float restitution = 0.0f, float isSensor = false)
			: Shape(shapeType), Offset(offset), BoxSize(boxSize), CircleRadius(1.0f)
			, Density(density), Friction(friction), Restitution(restitution), IsSensor(isSensor), Name() {};
		
		FixtureDefinition(ShapeType shapeType, const glm::vec2& offset = { 0.0f, 0.0f }, float circleRadius = 1.0f
						, float density = 1.0f, float friction = 0.5f, float restitution = 0.0f, float isSensor = false)
			: Shape(shapeType), Offset(offset), BoxSize{1.0f, 1.0f}, CircleRadius(circleRadius)
			, Density(density), Friction(friction), Restitution(restitution), IsSensor(isSensor), Name() {};
	};

	struct BoxColliderComponent
	{
		std::vector<FixtureDefinition> Fixtures = {};

		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;
		BoxColliderComponent(FixtureDefinition& fixture) {
			Fixtures.push_back(fixture);
		}
		BoxColliderComponent(glm::vec2 scale) {
			Dog::FixtureDefinition fixture;
			fixture.BoxSize = scale;
			Fixtures.push_back(fixture);
		}

		BoxColliderComponent(std::initializer_list<FixtureDefinition> fixtures) {
			for (const auto& fixture : fixtures) {
				Fixtures.push_back(fixture);
			}
		}

		void AddFixtures(FixtureDefinition fixture) {
			Fixtures.push_back(fixture);
			Box2dFixturesToAdd.push_back(fixture);
		}

		void AddFixtures(std::initializer_list<FixtureDefinition> fixtures) {
			for (const auto& fixture : fixtures) {
				Fixtures.push_back(fixture);
				Box2dFixturesToAdd.push_back(fixture);
			}
		}

		void RemoveFixture(const std::string& name) {
			Fixtures.erase(
				std::remove_if(
					Fixtures.begin(),
					Fixtures.end(),
					[&name](const FixtureDefinition& fd) { return fd.Name == name; }
				),
				Fixtures.end()
			);
			
			Box2dFixturesToRemove.push_back(name);
		}

		std::unordered_map<std::string, b2Fixture*> Box2dFixtures;
		std::vector<FixtureDefinition> Box2dFixturesToAdd;
		std::vector<std::string> Box2dFixturesToRemove;

	private:
		friend Scene;
		friend Physics;
		bool m_DoInit = true;
	};

	// -------------------------------- //
	// Scripting Components ----------- //

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		std::unordered_map<std::string, std::any> TemporaryVariables;

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}

		void SetVariable(const std::string& name, const std::any& value) {
        if (Instance) {
			throw std::runtime_error("Use SetVariable from ScriptableEntity when the script has already initialized (one frame after binding).");
			//Instance->SetVariable(name, value);
        } else {
            TemporaryVariables[name] = value;
        }
    }
	};

	// -------------------------------- //

}