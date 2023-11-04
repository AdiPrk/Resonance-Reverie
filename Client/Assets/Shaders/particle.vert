#version 330 core

layout (location = 0) in vec2 vertex; // <vec2 position>
layout (location = 1) in vec2 offset; // Instance data
layout (location = 2) in vec4 color;  // Instance data
layout (location = 3) in float scale; // Instance data

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projectionView;

void main()
{
    TexCoords = vertex;
    ParticleColor = color;
    gl_Position = projectionView * vec4(vertex * scale + offset, 0.0, 1.0);
}