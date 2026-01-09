#version 330 core

in vec2 exTexcoords;
out vec4 FragColor;

uniform sampler2D tex;

void main() {
    // debugging
    vec3 c = texture(tex, exTexcoords).rgb;
    FragColor = vec4(c, 1.0);
}