#version 330 core 

out vec4 FragColor;
in vec2 exTexcoords;

uniform sampler2D screenSampler;
uniform float gamma;


void main() {
	vec4 fragment = texture(screenSampler, exTexcoords);
	FragColor.rgb = pow(fragment.rgb, vec3(1.0f / gamma));
}

