#pragma once

#include <PCH/pch.h>

#include "Engine/Graphics/Shader/shader.h"
#include "Engine/Graphics/Texture/texture.h"

namespace Dog {

    class SpriteRenderer
    {
    public:
        // Constructor (inits shaders/shapes)
        SpriteRenderer();
        ~SpriteRenderer();

        // Renders a defined quad textured with given sprite
        void DrawSprite(const std::string& textureName, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotation = 0.0f, glm::vec4 color = glm::vec4(1.0f), glm::vec2 repetition = glm::vec2(0));
        void DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotation = 0.0f, glm::vec4 color = glm::vec4(1.0f), glm::vec2 repetition = glm::vec2(0));
        void DrawSpriteFrame(Texture2D& texture, unsigned frame, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotation = 0.0f, glm::vec4 color = glm::vec4(1.0f), glm::vec2 repetition = glm::vec2(0));

        // Draw a line between two points
        void DrawLine(glm::vec2 p1, glm::vec2 p2, float thickness, const std::string& textureName);
        void DrawLine(glm::vec2 p1, glm::vec2 p2, float thickness, Texture2D& texture);

    private:
        // Render state

        unsigned int quadVAO;

        // Initializes and configures the quad's buffer and vertex attributes
        void initRenderData();
    };

}