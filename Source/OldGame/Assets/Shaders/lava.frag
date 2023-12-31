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
    // vec2 uvpos = (TexCoords - 0.5) * 2.0; // -1 to 1
    // 
    // if (uvpos.y < -1 + sin(uvpos.x * 5 + iTime * 0.02) * 0.1 + 0.05) {
    //     discard;
    // }

    vec2 col;
    float t = iTime*.1;
    vec2 uv = (TexCoords - 0.5) * 2.0+vec2(t,t*2.0);
    float factor = 1.5;
    vec2 v1;
    for(int i=0;i<12;i++)
    {
        uv *= -factor*factor;
        v1 = uv.yx/factor;
        uv += sin(v1+col+t*10.0)/factor;
        col += vec2(sin(uv.x-uv.y+v1.x-col.y),sin(uv.y-uv.x+v1.y-col.x));
    }

    vec3 lavaColor = vec3(col.x+4.0,col.x-col.y/2.0,col.x/5.0)/2.0;
    vec3 red = vec3(1.0, 0.0, 0.0);
    fragColor = vec4(red * 0.7 + lavaColor * 0.3, 1.0);
}