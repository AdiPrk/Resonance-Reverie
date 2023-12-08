#version 430 core

in vec2 TexCoords;
out vec4 fragColor;

layout (std140) uniform Time
{
    float iTime;
};

void main()
{
    vec2 col;
    float t = iTime*.02;
    vec2 uv = (TexCoords - 0.5) * 16.0;
    float factor = 1.5;
    vec2 v1;
    for(int i=0;i<12;i++)
    {
        uv *= -factor*factor;
        //uv.x *= 2.0;
        v1 = uv.yx/factor;
        uv += sin(v1+col+t*10.0)/factor;
        col += vec2(sin(uv.x-uv.y+v1.x-col.y),sin(uv.y-uv.x+v1.y-col.x));
    }
    fragColor = vec4(vec3(col.x+1.0,col.x-col.y/2.0,col.x*2.0)/2.0,0.035);
}