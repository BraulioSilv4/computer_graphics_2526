#version 330 core 

out vec4 FragColor;
in vec2 exTexcoords;

uniform sampler2D screenSampler;
uniform float gamma;


void main() {
	vec4 fragment = texture(screenSampler, exTexcoords);

	float exposure = 1.0f;
	vec3 toneMapped = vec3(1.0f) - exp(-fragment.rgb * exposure);
	//	color = color / (color + vec3(1.0)); // Reinhard tone mapping

	FragColor = vec4(pow(toneMapped, vec3(1.0f / gamma)), 1.0);
}

