#version 330 core

in vec3 exPosition;

uniform vec4 LightColor;

out vec4 FragmentColor;

void main(void)
{
    FragmentColor = LightColor;
}
