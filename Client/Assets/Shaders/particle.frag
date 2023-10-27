#version 330 core

in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 color;
in vec4 fragPosition;

uniform sampler2D sprite;

void main()
{
    // Discard if the position is out of the NDC range
    if (fragPosition.x < -1.0 || fragPosition.x > 1.0 ||
        fragPosition.y < -1.0 || fragPosition.y > 1.0) {
        discard;
    }

    color = (texture(sprite, TexCoords) * ParticleColor);
}