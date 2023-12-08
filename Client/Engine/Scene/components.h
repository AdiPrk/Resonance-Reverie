#pragma once

#include <PCH/pch.h>

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
	glm::vec2 position;
	glm::vec2 scale;
	float rotation;

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const glm::vec2& pos, const glm::vec2& size, float rot)
		: position(pos)
		, scale(size)
		, rotation(rot) {}
};

struct SpriteRendererComponent
{
	glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

	SpriteRendererComponent() = default;
	SpriteRendererComponent(const SpriteRendererComponent&) = default;
	SpriteRendererComponent(const glm::vec4& color)
		: Color(color) {}
};