#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform bool  chaos;
uniform bool  confuse;
uniform bool  shake;
uniform float iTime;

// Get a pseudo random number
float randNum(vec2 p) {
    p = fract(p * vec2(142.215, 501.253));
    p += dot(p, p + 23.45);
    return fract(p.x * p.y);
}

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
    if (shake)
    {
        float strength = 0.003f;
        gl_Position.x += cos(iTime * 20) * strength;        
        gl_Position.y += cos(iTime * 30) * strength;        
    }
}  