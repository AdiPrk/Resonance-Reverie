#version 430 core
#extension GL_ARB_bindless_texture : require

in vec2 TexCoords;
in vec2 worldPos;
out vec4 fragColor;

uniform sampler2D textureHandle;
uniform vec4 spriteColor;
uniform vec2 repetition;
uniform vec2 offset;

layout (std140) uniform Time
{
    float iTime;
};

struct Light {
    vec2 position;
    float radius;
    float intensity;
};

#define MAX_LIGHTS 5
uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform float ambientLightIntensity;

// Simple hash function for dithering
float randdither(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{    
    float total_light = ambientLightIntensity;
    for (int i = 0; i < numLights; i++) {
        float distance = length(worldPos - lights[i].position);
    
        float spotEffect = clamp((lights[i].radius - distance) / lights[i].radius, 0.0, 1.0);
        spotEffect *= spotEffect * 5;

        total_light += lights[i].intensity * spotEffect;
    }

    vec2 newTexCoords = (TexCoords + offset) * repetition;

    fragColor = spriteColor * texture(textureHandle, newTexCoords);

    // Apply light
    float dither = randdither(newTexCoords) * 0.05;
    total_light += dither;
    fragColor.rgb *= total_light;
}