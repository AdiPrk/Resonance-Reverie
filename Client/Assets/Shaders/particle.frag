#version 330 core
#extension GL_ARB_bindless_texture : require

in vec2 TexCoords;
in vec4 ParticleColor;

out vec4 fragColor;

uniform sampler2D  textureHandle;

void main()
{
    fragColor = (texture(textureHandle, TexCoords) * ParticleColor);
}