#version 430 core

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec2 worldPos;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    mat4 projectionView;
};

uniform mat4 model;

void main()
{
    TexCoords = vertex.zw;
    vec4 worldPosition = model * vec4(vertex.xy, 0.0, 1.0);
    gl_Position = projectionView * worldPosition;
    worldPos = worldPosition.xy;
}