#version 330 core 

out vec4 FragColor;
in vec2 exTexcoords;

uniform sampler2D screenSampler;
uniform float gamma;


void main() {
	vec4 fragment = texture(screenSampler, exTexcoords);

	float exposure = 1.5f;
	vec3 toneMapped = vec3(1.0f) - exp(-fragment.rgb * exposure);

	FragColor = vec4(pow(toneMapped, vec3(1.0f / gamma)), 1.0);
}

