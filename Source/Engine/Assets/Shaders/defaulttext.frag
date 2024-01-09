#version 430 core
#extension GL_ARB_bindless_texture : require

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D text;
uniform vec3 textColor;
uniform int       edge_kernel[9];
uniform vec2      offsets[9];

void main()
{    
    vec3 samples[9];
    for(int i = 0; i < 9; i++) {
        samples[i] = vec3(texture(text, TexCoords.st + offsets[i]));
    }

    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    
    //fragColor = vec4(0.0f);
    
    //for(int i = 0; i < 9; i++) {
    //    fragColor += vec4(samples[i] * edge_kernel[i], 0.0f);
    //}
    //fragColor.a = 1.0f;

    fragColor = vec4(textColor, 1.0) * sampled;
}  