#version 330 core

layout(location = 1) in vec3 inPosition;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

out vec3 exPosition;

void main() {
	exPosition = inPosition;

	mat4 rotView = mat4(mat3(ViewMatrix));
	vec4 pos = ProjectionMatrix * rotView * vec4(inPosition, 1.0);
	gl_Position = pos.xyww;
}