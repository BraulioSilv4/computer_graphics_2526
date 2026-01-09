#version 330 core
out vec4 FragColor;

in vec3 exPosition;

uniform sampler2D equirectangularSampler;
uniform float gamma;

const vec2 invAtan = vec2(0.1591, 0.3183); 

vec2 sampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{    
    vec2 uv = sampleSphericalMap(normalize(exPosition));
    vec3 color = texture(equirectangularSampler, uv).rgb;

    // Raw HDR output tone mapping is done in post-process framebuffer
    FragColor = vec4(color, 1.0);
}	