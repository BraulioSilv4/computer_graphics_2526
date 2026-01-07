#version 330 core
out vec4 FragColor;

in vec3 exPosition;

uniform samplerCube cubeMapSampler;
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
//	vec3 color = textureLod(cubeMapSampler, exPosition, 0.5).rgb;
//
//	FragColor = vec4(color, 1.0);
    vec2 uv = sampleSphericalMap(normalize(exPosition));
    vec3 color = texture(equirectangularSampler, uv).rgb;

    // Raw HDR output - tone mapping done in post-process
    FragColor = vec4(color, 1.0);
}	