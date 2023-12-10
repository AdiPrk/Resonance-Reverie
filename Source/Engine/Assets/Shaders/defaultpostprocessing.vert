#version 430 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform bool  chaos;
uniform bool  confuse;
uniform bool  shake;
uniform bool disco;
uniform float shakeScale;

layout (std140) uniform Time
{
    float iTime;
};

void main()
{
    gl_Position = vec4(vertex.xy, 0.0f, 1.0f); 
    vec2 texture = vertex.zw;
    if (chaos)
    {
        float strength = 0.3;
        vec2 pos = vec2(texture.x + sin(iTime) * strength, texture.y + cos(iTime) * strength);        
        TexCoords = pos;
    }
    else if (confuse)
    {
        TexCoords = vec2(1.0 - texture.x, 1.0 - texture.y);
    }
    else
    {
        TexCoords = texture;
    }

    if (disco) {
        gl_Position.xy *= 1.35;

        float rotAng = iTime * 0.5;
        float c = cos(rotAng);
        float s = sin(rotAng);
        mat2 rot = mat2(c, -s, s, c);
        gl_Position.xy *= rot;
    }

    if (shake)
    {
        float strength = shakeScale / 10.0;
        gl_Position.x += cos(iTime * 50) * strength;        
        gl_Position.y += cos(iTime * 60) * strength;        
    }
}  