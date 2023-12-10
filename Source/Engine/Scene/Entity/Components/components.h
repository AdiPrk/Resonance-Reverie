#pragma once

#include <Engine/PCH/pch.h>
#include <Engine/Graphics/Texture/texture.h>
#include <Engine/ResourceManager/resourceManager.h>

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
		glm::vec2 position = { 0.0f, 0.0f };
		glm::vec2 scale = { 50.0f, 50.0f };
		float rotation = 0;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec2& pos, const glm::vec2& size, float rot)
			: position(pos)
			, scale(size)
			, rotation(rot) {}
	};

	// -------------------------------- //
	// Graphics Components ------------ //

	struct QuadRendererComponent
	{
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };

		QuadRendererComponent() = default;
		QuadRendererComponent(const QuadRendererComponent&) = default;
		QuadRendererComponent(const glm::vec4& col)
			: color(col) {}
	};

	struct SpriteRendererComponent
	{
		Texture2D texture;
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const std::string& texName, const glm::vec4& col = { 1.0f, 1.0f, 1.0f, 1.0f })
			: texture(ResourceManager::GetTexture(texName))
			, color(col) {}
		SpriteRendererComponent(const Texture2D& tex, const glm::vec4& col = { 1.0f, 1.0f, 1.0f, 1.0f })
			: texture(tex)
			, color(col) {}
	};

	struct TextRendererComponent
	{
		std::string text{};
		float scale = 1;
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		bool centered = true;
		bool diagetic = true;

		TextRendererComponent() = default;
		TextRendererComponent(const TextRendererComponent&) = default;
		TextRendererComponent(const std::string& text, float size = 1, const glm::vec4& col = { 1.0f, 1.0f, 1.0f, 1.0f }, bool alignCenter = true, bool useCamera = true)
			: text(text)
			, scale(size)
			, color(col)
			, centered(alignCenter)
			, diagetic(useCamera) {}
	};

	// -------------------------------- //
	// Physics Components ------------- //

	struct RigidbodyComponent
	{
		enum class BodyType { Static = 0, Dynamic = 1, Kinematic = 2};
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;
		b2Body* Body;

		RigidbodyComponent() = default;
		RigidbodyComponent(const RigidbodyComponent&) = default;
	};

	struct BoxColliderComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 25.0f, 25.0f };

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		b2Fixture* Fixture;

		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;
	};

	// -------------------------------- //
	// Scripting Components ----------- //

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	// -------------------------------- //

}