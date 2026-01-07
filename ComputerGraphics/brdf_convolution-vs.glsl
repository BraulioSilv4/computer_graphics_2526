#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;

out vec3 exPosition;
out vec2 exTexcoord;

void main(void)
{	
	exPosition = inPosition;
	exTexcoord = inTexcoord;
	vec4 MCPosition = vec4(inPosition, 1.0);
	gl_Position = vec4(inPosition, 1.0);
}