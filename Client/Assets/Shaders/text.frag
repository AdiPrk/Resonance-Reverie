#version 430 core
#extension GL_ARB_bindless_texture : require

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    fragColor = vec4(textColor, 1.0) * sampled;
}  