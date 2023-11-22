#version 430 core
#extension GL_ARB_bindless_texture : require

in  vec2  TexCoords;
out vec4  fragColor;
  
uniform sampler2D  scene;
uniform vec2      offsets[9];
uniform int       edge_kernel[9];
uniform float     blur_kernel[9];

uniform bool  chaos;
uniform bool  confuse;
uniform bool  shake;
uniform bool  invert;
uniform bool  blur;
uniform bool  greyscale;
uniform float blurStrength;

void main()
{
    vec4 color = vec4(0.0f);
    vec3 samples[9];
    // sample from texture offsets if using convolution matrix
    if(chaos || shake || blur)
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
        for(int i = 0; i < 9; i++)
            color += vec4(samples[i] * blur_kernel[i], 0.0f);
        color.a = 1.0f;
        vec4 baseColor =  texture(scene, TexCoords);
        color = color * 0.4 + baseColor * 0.6;
    }
    else
    {
        color =  texture(scene, TexCoords);
    }

    // Blur effect
    if (blur)
    {
        // blur
        vec4 col = vec4(0.0f);

        for(int i = 0; i < 9; i++)
            col += vec4(samples[i] * blur_kernel[i], 0.0f);
        col.a = 1.0f;
        float bluramt = min(1.0, max(0.0, blurStrength));
        color = color * (1.0 - bluramt) + col * (bluramt);
    }

    // Invert color effect
    if (invert) {
        color = vec4((1.0 - color.rgb), 1);//vec4(vec3(color.rgb * total_light), color.a);
    }

    // Greyscale effect
    if (greyscale) {
        float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
        color = vec4(average, average, average, 1.0);
    }

    fragColor = color;//vec4(vec3(color.rgb * total_light), color.a);
}