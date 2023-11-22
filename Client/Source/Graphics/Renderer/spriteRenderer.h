#pragma once

#include <PCH/pch.h>

#include "Source/Graphics/Shader/shader.h"
#include "Source/Graphics/Texture/texture.h"

class SpriteRenderer
{
public:
    // Constructor (inits shaders/shapes)
    SpriteRenderer(Shader& shader);
    ~SpriteRenderer();

    // set shader
    void SetShader(Shader& shader);

    // Renders a defined quad textured with given sprite
    void DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec4 color = glm::vec4(1.0f), glm::vec2 repetition = glm::vec2(0));
    
    // Draw a line between two points
    void DrawLine(glm::vec2 p1, glm::vec2 p2, float thickness, Texture2D& texture);

private:
    // Render state
    Shader       shader;
    unsigned int quadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};