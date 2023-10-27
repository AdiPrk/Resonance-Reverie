#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform bool diagetic; // true if affected by camera

void main()
{
    if (diagetic) {
        gl_Position = projection * view * vec4(vertex.xy, 0.0, 1.0);
    } else {
        gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    }

    TexCoords = vertex.zw;
}