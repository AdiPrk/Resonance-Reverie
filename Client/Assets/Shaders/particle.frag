#version 330 core

in vec2 TexCoords;
in vec4 ParticleColor;
in vec4 fragPosition;

out vec4 fragColor;

uniform sampler2D sprite;

void main()
{
    fragColor = (texture(sprite, TexCoords) * ParticleColor);
}