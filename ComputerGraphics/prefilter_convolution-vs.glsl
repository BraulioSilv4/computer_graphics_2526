#version 330 core 

layout(location = 1) in vec3 inPosition;

out vec3 exPosition;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{	
	exPosition = inPosition;

	vec4 MCPosition = vec4(inPosition, 1.0);
	gl_Position = ProjectionMatrix * ViewMatrix * MCPosition;
}