#version 330 core

layout(location = 1) in vec3 inPosition;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexcoord;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitTangent;	

out vec3 exPosition;	// World Space position
out vec2 exTexcoord;
out vec3 exNormal;		// World Space normal
out vec3 exTangent; 	// World Space tangent
out vec3 exBitTangent;  // World Space bitangent

uniform mat4 ModelMatrix;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

void main(void)
{
	mat3 NormalMatrix = transpose(inverse(mat3(ModelMatrix)));

	vec4 worldPosition = ModelMatrix * vec4(inPosition, 1.0);
	exPosition = worldPosition.xyz;
	
	exNormal = normalize(NormalMatrix * inNormal);
	exTangent = normalize(NormalMatrix * inTangent);
	exBitTangent = normalize(NormalMatrix * inBitTangent);
	
	exTexcoord = inTexcoord;

	gl_Position = ProjectionMatrix * ViewMatrix * worldPosition;
}
