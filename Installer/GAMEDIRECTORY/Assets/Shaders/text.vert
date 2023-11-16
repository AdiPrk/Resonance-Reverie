#version 430 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    mat4 projectionView;
};

uniform bool diagetic; // true if affected by camera

void main()
{
    if (diagetic) {
        gl_Position = projectionView * vec4(vertex.xy, 0.0, 1.0);
    } else {
        gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    }

    TexCoords = vertex.zw;
}