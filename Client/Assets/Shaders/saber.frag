#version 430 core

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D image;
uniform vec4 spriteColor;

layout (std140) uniform Time
{
    float iTime;
};

void main()
{
    vec2 uv = (TexCoords - 0.5) * 2.0;

    float rotAng = 1.57075;
    float c = cos(rotAng);
    float s = sin(rotAng);
    mat2 rot = mat2(c, -s, s, c);
    
    uv *= rot;
        
    float d = 0.0;
    
    for (float i = 0.0; i < 25.0; i++) {
        float t = sin(i * 1.0 + iTime * 5.0 + uv.y * 4.0) * 0.45;
        uv.x += t;
        
    	d += abs(2.5 / uv.x) * 0.3 * (0.25 + sin(uv.y + iTime * 3.0) * 0.01);
    }
    
    vec3 color = vec3(0.01, 0.07, 0.12) * d;
    
    if (color.b < 0.95 || color.r < 0.2) {
        discard;
    }
    else {
        fragColor = vec4(color, 1.0);
    }
}