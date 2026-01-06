#version 330 core
out vec4 FragColor;

in vec3 exPosition;

uniform samplerCube cubeMapSampler;
uniform float gamma;

void main()
{    
	vec3 color = texture(cubeMapSampler, exPosition).rgb;

	FragColor = vec4(color, 1.0);
}	