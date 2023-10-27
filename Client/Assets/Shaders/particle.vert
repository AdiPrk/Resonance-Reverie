#version 330 core

layout (location = 0) in vec2 vertex; // <vec2 position>
layout (location = 1) in vec2 offset; // Instance data
layout (location = 2) in vec4 color;  // Instance data
layout (location = 3) in float scale; // Instance data

out vec2 TexCoords;
out vec4 ParticleColor;
out vec4 fragPosition;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = vertex;
    ParticleColor = color;
    fragPosition = projection * view * vec4((vertex.xy * scale) + offset, 0.0, 1.0);
    gl_Position = fragPosition;
}