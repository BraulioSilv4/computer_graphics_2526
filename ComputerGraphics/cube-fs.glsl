#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;

out vec4 FragmentColor;

uniform vec3 objectColor;
uniform sampler2D diffSampler;

vec3 diffuseColor(void) {
    vec3 N = normalize(exNormal);
    vec3 direction = vec3(1.0, 0.5, 0.25);
    float intensity = max(dot(direction, N), 0.0);
    return objectColor * intensity;
}

void main(void)
{
    FragmentColor = texture(diffSampler, exTexcoord);
}
