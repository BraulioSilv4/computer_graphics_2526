#version 330 core
out vec4 FragColor;

in vec3 exTexcoord;

uniform samplerCube cubeMapSampler;

void main()
{    
	FragColor = texture(cubeMapSampler, exTexcoord);
}	