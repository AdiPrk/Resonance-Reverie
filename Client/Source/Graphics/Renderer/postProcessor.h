#pragma once

#include <PCH/pch.h>

#include <Source/Graphics/Texture/texture.h>
#include <Source/Graphics/Shader/shader.h>
#include "spriteRenderer.h"

// PostProcessor hosts all PostProcessing effects for the Breakout
// Game. It renders the game on a textured quad after which one can
// enable specific effects by enabling either the Confuse, Chaos or 
// Shake boolean. 
// It is required to call BeginRender() before rendering the game
// and EndRender() after rendering the game for the class to work.

struct ViewportInfo;

class PostProcessor
{
public:
    // state
    Shader PostProcessingShader;
    Texture2D Texture;
    unsigned int Width, Height;
    // options
    bool Confuse, Chaos, Shake;
    float shakeTime = 0.0f;
    // constructor
    PostProcessor(Shader shader, unsigned int width, unsigned int height);
    // update
    void Update(float dt);
    // prepares the postprocessor's framebuffer operations before rendering the game
    void BeginRender();
    // should be called after rendering the game, so it stores all the rendered data into a texture object
    void EndRender();
    // renders the PostProcessor texture quad (as a screen-encompassing large sprite)
    void Render(float time);
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