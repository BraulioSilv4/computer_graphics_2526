#version 330 core
out vec4 FragColor;

in vec3 exTexcoord;

uniform samplerCube cubeMapSampler;
uniform float gamma;

void main()
{    
	vec3 color = texture(cubeMapSampler, exTexcoord).rgb;
	FragColor = vec4(pow(color, vec3(gamma)), 1.0);
}	