#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;

out vec4 FragmentColor;

uniform vec3 objectColor;

uniform sampler2D diffSampler;
uniform sampler2D normalSampler;
uniform sampler2D roughSampler;
uniform sampler2D metalSampler;


vec3 diffuseColor(void) {
    vec3 N = normalize(exNormal);
    vec3 direction = vec3(1.0, 0.5, 0.25);
    float intensity = max(dot(direction, N), 0.0);
    return objectColor * intensity;
}

void main(void)
{
    vec3 normalMap = texture(normalSampler, exTexcoord).rgb;
    float roughMap = texture(roughSampler, exTexcoord).r;

    //FragmentColor = vec4(normalMap, 1.0);
    //FragmentColor = texture(diffSampler, exTexcoord);
    FragmentColor = vec4(roughMap, 0.0, 0.0, 1.0);
}
