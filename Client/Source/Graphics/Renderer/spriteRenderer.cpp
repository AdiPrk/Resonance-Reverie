#include <PCH/pch.h>

#include "spriteRenderer.h"

#include "Source/Graphics/Shader/shader.h"
#include "Source/Graphics/Texture/texture.h"
#include <Source/Graphics/ResourceManager/resourceManager.h>

SpriteRenderer::SpriteRenderer(Shader& shader)
{
    this->shader = shader;
    this->initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::SetShader(Shader& shader)
{
    this->shader = shader;
    shader.Use();
}

void SpriteRenderer::DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec4 color, glm::vec2 repetition)
{
    // prepare transformations
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

    model = glm::scale(model, glm::vec3(size, 1.0f));

    this->shader.SetMatrix4("model", model);

    // render textured quad
    this->shader.SetVector4f("spriteColor", color);

    if (repetition.x != 0 && repetition.y != 0) {
        this->shader.SetVector2f("repetition", size.x / repetition.x, size.y / repetition.y);
    }
    else {
        this->shader.SetVector2f("repetition", 1.0f, 1.0f);
    }

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    ResourceManager::BindVAO(this->quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SpriteRenderer::DrawLine(glm::vec2 p1, glm::vec2 p2, float thickness, Texture2D& texture)
{
    glm::vec2 direction = p2 - p1;
    float distance = glm::length(direction);
    float angle = atan2(direction.y, direction.x); // Angle in radians

    glm::vec2 lineSize(distance, thickness); // 1.0f is the thickness of the line
    glm::vec2 linePosition = p1 + direction / 2.0f - lineSize / 2.0f;

    // Drawing the rectangle (line) with the calculated position, size, and rotation
    DrawSprite(texture, linePosition, lineSize, angle);
}

void SpriteRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        // pos
        0.0f, 1.0f, 0.0f, 1.0f, // Top-left vertex
        0.0f, 0.0f, 0.0f, 0.0f, // Bottom-left vertex
        1.0f, 1.0f, 1.0f, 1.0f, // Top-right vertex
        1.0f, 0.0f, 1.0f, 0.0f  // Bottom-right vertex
    };

    //float vertices[] = {
    //    // pos
    //    -0.5f,  0.5f, 0.0f, 1.0f, // Top-left vertex
    //    -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left vertex
    //     0.5f,  0.5f, 1.0f, 1.0f, // Top-right vertex
    //     0.5f, -0.5f, 1.0f, 0.0f  // Bottom-right vertex
    //};

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    ResourceManager::BindVAO(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    ResourceManager::BindVAO(0);
}