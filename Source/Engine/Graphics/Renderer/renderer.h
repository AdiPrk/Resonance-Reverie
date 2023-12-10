#pragma once

#include <Engine/PCH/pch.h>

namespace Dog {

	class SpriteRenderer;
	class TextRenderer;
	class PostProcessor;
	class Shader;
	class Texture2D;
	class Camera;

	class Renderer {
	public:
		static void Init(unsigned int width, unsigned int height);

		static void SetShader(Shader& shader);
		static void SetShader(const std::string& shaderName);
		static void LoadText(const std::string& fontPath, unsigned int fontSize);

		static Shader& GetActiveShader() { return m_ActiveShader; }

		// Clear the screen
		static void Clear(float r, float g, float b, float a);
		static void Clear(const glm::vec4& color = m_ClearColor);
		static void ClearColorAndDepthBuffer();

		// Render a quad. Uses a default internal shader.
		static void DrawQuad(glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotation = 0.0f, glm::vec4 color = glm::vec4(1.0f));

		
		// Renders a quad textured with given sprite. Uses the last set shader.
		static void DrawSprite(const std::string& textureName, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotation = 0.0f, glm::vec4 color = glm::vec4(1.0f), glm::vec2 repetition = glm::vec2(0));
		static void DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotation = 0.0f, glm::vec4 color = glm::vec4(1.0f), glm::vec2 repetition = glm::vec2(0));
		static void DrawSpriteFrame(Texture2D& texture, unsigned frame, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotation = 0.0f, glm::vec4 color = glm::vec4(1.0f), glm::vec2 repetition = glm::vec2(0));

		// Draw a line between two points. Uses the last set shader.
		static void DrawLine(glm::vec2 p1, glm::vec2 p2, float thickness, const std::string& textureName);
		static void DrawLine(glm::vec2 p1, glm::vec2 p2, float thickness, Texture2D& texture);

		// Renders a string of text. Uses the last loaded font.
		static void RenderText(const std::string& text, float x, float y, float scale, bool centered = false, glm::vec3 color = glm::vec3(1.0f), bool diagetic = false);

		// post processing
		static void BeginPostProcessing();
		static void UpdatePostProcessing(float dt);
		static void EndPostProcessing();

		// Getters
		static PostProcessor* GetPostProcessor() { return m_PostProcessor; }
		static const glm::vec4& GetClearColor() { return m_ClearColor; }
		static void Update(float dt);
		
	private:
		static SpriteRenderer* m_SpriteRenderer;
		static TextRenderer* m_TextRenderer;
		static PostProcessor* m_PostProcessor;
		static Camera m_Camera;
		static Shader m_ActiveShader;
		static glm::vec4 m_ClearColor;
	};

}