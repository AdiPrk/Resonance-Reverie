#version 330 core
#extension GL_ARB_bindless_texture : require

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D textureHandle;
uniform vec4 spriteColor;
uniform vec2 repetition;
uniform float iTime;

void main()
{    
    vec2 newTexCoords = TexCoords * repetition;

    fragColor = vec4(spriteColor) * texture(textureHandle, newTexCoords);
}