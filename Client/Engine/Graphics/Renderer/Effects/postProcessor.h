#pragma once

#include <PCH/pch.h>

#include <Engine/Graphics/Texture/texture.h>
#include <Engine/Graphics/Shader/shader.h>
struct ViewportInfo;

// PostProcessor hosts all PostProcessing effects for the game
class PostProcessor
{
public:
    // state
    Shader PostProcessingShader;
    Texture2D Texture;
    unsigned int Width, Height;
    // options
    bool Confuse, Chaos, Shake, Invert, Greyscale, Blur;
    float shakeTime = 0.0f;
    float shakeScale = 0.05f;
    float blurTime = 0.0f;
    float blurStrength = 0.25f;
    float invertTime = 0.0f;
    float greyscaleTime = 0.0f;
    // constructor
    PostProcessor(unsigned int width, unsigned int height);
    // update
    void Update(float dt);
    void UpdateEffect(float& effectTime, bool& effectFlag, float dt);
    // prepares the postprocessor's framebuffer operations before rendering the game
    void BeginRender();
    // should be called after rendering the game, so it stores all the rendered data into a texture object
    void EndRender();
    // renders the PostProcessor texture quad (as a screen-encompassing large sprite)
    void Render();
private:
    // render state
    unsigned int MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
    unsigned int RBO; // RBO is used for multisampled color buffer
    unsigned int VAO;
    // initialize quad for rendering postprocessing texture
    void initRenderData();

    // A struct to hold the opengl viewport information
    struct ViewportInfo {
        GLint x, y, width, height;

        ViewportInfo() : x(), y(), width(), height() {};
        ViewportInfo(GLint info[4]) : x(info[0]), y(info[1]), width(info[2]), height(info[3]) {};
    };

    ViewportInfo m_ViewportInfo;
};