#pragma once

#include <PCH/pch.h>

namespace Dog {

	class SpriteRenderer;
	class TextRenderer;
	class PostProcessor;
	class Shader;
	class Texture2D;

	class Renderer {
	public:
		static void Init(unsigned int width, unsigned int height);

		static void SetShader(Shader& shader);
		static void SetShader(const std::string& shaderName);
		static void LoadText(const std::string& fontPath, unsigned int fontSize);

		static Shader& GetActiveShader() { return m_ActiveShader; }

		// Renders a defined quad textured with given sprite
		static void DrawSprite(const std::string& textureName, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotation = 0.0f, glm::vec4 color = glm::vec4(1.0f), glm::vec2 repetition = glm::vec2(0));
		static void DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotation = 0.0f, glm::vec4 color = glm::vec4(1.0f), glm::vec2 repetition = glm::vec2(0));
		static void DrawSpriteFrame(Texture2D& texture, unsigned frame, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotation = 0.0f, glm::vec4 color = glm::vec4(1.0f), glm::vec2 repetition = glm::vec2(0));

		// Draw a line between two points
		static void DrawLine(glm::vec2 p1, glm::vec2 p2, float thickness, const std::string& textureName);
		static void DrawLine(glm::vec2 p1, glm::vec2 p2, float thickness, Texture2D& texture);

		// renders a string of text using the precompiled list of characters
		static void RenderText(const std::string& text, float x, float y, float scale, bool centered = false, glm::vec3 color = glm::vec3(1.0f), bool diagetic = false);

		// post processing
		static void BeginPostProcessing();
		static void UpdatePostProcessing(float dt);
		static void EndPostProcessing();
		static PostProcessor* GetPostProcessor() { return m_PostProcessor; }
	private:
		static SpriteRenderer* m_SpriteRenderer;
		static TextRenderer* m_TextRenderer;
		static PostProcessor* m_PostProcessor;
		static Shader m_ActiveShader;
	};

}