#version 330 core

layout(location = 1) in vec3 inPosition;

out vec3 exTexcoord;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main() {
	vec4 pos = ProjectionMatrix * ViewMatrix * vec4(inPosition, 1.0);
	gl_Position = pos.xyww;
	exTexcoord = vec3(inPosition.x, inPosition.y, -inPosition.z);	
}