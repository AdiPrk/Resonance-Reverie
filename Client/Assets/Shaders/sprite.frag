#version 330 core

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D image;
uniform vec4 spriteColor;
uniform vec2 repetition;
uniform float iTime;

void main()
{    
    vec2 newTexCoords;
    newTexCoords.x = TexCoords.x * repetition.x;
    newTexCoords.y = TexCoords.y * repetition.y;

    fragColor = vec4(spriteColor) * texture(image, newTexCoords);
}