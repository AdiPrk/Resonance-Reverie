#version 430 core
#extension GL_ARB_bindless_texture : require
#define PI 3.14159

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
uniform bool  disco;

layout (std140) uniform Time
{
    float iTime;
};

vec2 path(float t) {
    float a = sin(t*.2+1.5),b=sin(t*.2);
    return vec2(a*2., a*b);
}

mat2 r2d(float a) {
    float c=cos(a),s=sin(a);
    return mat2(c, s, -s, c);
}

void mo(inout vec2 p, vec2 d) {
    p.x = abs(p.x) - d.x;
    p.y = abs(p.y) - d.y;
    if(p.y>p.x)p=p.yx;
}

float g=0.;
float de(vec3 p) {
   
    vec3 q = p;
    q.x += q.z*.1;
    q.z += iTime*.1;
    q = mod(q-1., 2.)-1.;
    float s = length(q) - .001 + sin(iTime*30.)*.005;
    
    p.xy -= path(p.z);
    
    p.xy *= r2d(p.z*.9);
    mo(p.xy, vec2(.6, .12));
    mo(p.xy, vec2(.9, .2));
    
    p.xy *= r2d(p.z*.5);
    
    mo(p.zy, vec2(.1, .2));
    p.x = abs(p.x) - .4;
    float d = length(p.xy) - .02 - (.5+.5*sin(p.z))*.05;
    
    d = min(d, s);
    
    
    g+=.01/(.01+d*d);
    return d;
}

void main()
{
    vec4 color = vec4(0.0f);
    vec3 samples[9];
    // sample from texture offsets if using convolution matrix
    if(chaos || shake || blur || disco)
        for(int i = 0; i < 9; i++)
            samples[i] = vec3(texture(scene, TexCoords.st + offsets[i]));

    // process effects
    if (chaos || disco)
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

    if (disco) {
        vec2 uv = (TexCoords - 0.5) * 2.0;
    
        float dt = iTime * 6.;
        vec3 ro = vec3(0,0, -3. + dt);
        vec3 ta = vec3(0, 0, dt);
    
        ro.xy += path(ro.z);
        ta.xy += path(ta.z);
    
        vec3 fwd = normalize(ta -ro);
        vec3 left = cross(vec3(0,1,0),fwd);
        vec3 up = cross(fwd, left);
    
        vec3 rd = normalize(fwd + left*uv.x+up*uv.y);

        vec3 p;
        float ri,t=0.;
        for(float i=0.;i<1.;i+=.01) {
    	    ri = i;
            p=ro+rd*t;
            float d = de(p);
            if(d<.001) break;
            t+=d*.2;
        }
	    vec3 bg =  vec3(.2, .6, .6)*.2; 
        vec3 col = mix(vec3(.4, .5, .5), bg,ri);
        col += g*.02;
    
        col = mix(col, bg, 1.-exp(-.01*t*t));

        vec4 newColor = vec4(col * abs(cos(vec3(0.2, 0.4, 0.6) * iTime * 25.)) + vec3(uv.xy, 0.),1.0);

        fragColor *= 1. / newColor; 
    }

}