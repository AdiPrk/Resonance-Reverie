#version 430 core
#extension GL_ARB_bindless_texture : require

in  vec2  TexCoords;
out vec4  fragColor;
  
uniform sampler2D  scene;
uniform vec2      offsets[9];
uniform int       edge_kernel[9];
uniform float     blur_kernel[9];

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

void main()
{
    vec4 color = vec4(0.0f);
    vec3 samples[9];
    // sample from texture offsets if using convolution matrix
    if(chaos || shake)
        for(int i = 0; i < 9; i++)
            samples[i] = vec3(texture(scene, TexCoords.st + offsets[i]));

    // process effects
    if (chaos)
    {           
        for(int i = 0; i < 9; i++)
            color += vec4(samples[i] * edge_kernel[i], 0.0f);
        color.a = 1.0f;
    }
    else if (confuse)
    {
        color = vec4(1.0 - texture(scene, TexCoords).rgb, 1.0);
    }
    else if (shake)
    {
        // blur
        // for(int i = 0; i < 9; i++)
        //     color += vec4(samples[i] * blur_kernel[i], 0.0f);
        // color.a = 1.0f;
        color =  texture(scene, TexCoords);
    }
    else
    {
        color =  texture(scene, TexCoords);
    }
    //float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    //fragColor = vec4(average, average, average, 1.0);

    fragColor = color;//vec4(vec3(color.rgb * total_light), color.a);
}