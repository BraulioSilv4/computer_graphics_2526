#version 330 core

in vec3 exPosition;

uniform samplerCube environmentSampler;

out vec4 FragColor;

const float PI = 3.14159265359;

void main(void) 
{	
	vec3 normal = normalize(exPosition);

	vec3 irradiance = vec3(0.0);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up = normalize(cross(normal, right));

	float sampleDelta = 0.02;
	float nrSamples = 0.0;
	for (float azimuth = 0.0; azimuth < 2 * PI; azimuth += sampleDelta) {
		for (float zenith = 0.0; zenith < 0.5 * PI; zenith += sampleDelta) {
			// Spherical to Cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(zenith) * cos(azimuth), sin(zenith) * sin(azimuth), cos(zenith));

			// Tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			irradiance += textureLod(environmentSampler, sampleVec, 3.0).rgb * cos(zenith) * sin(zenith);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	FragColor = vec4(irradiance, 1.0);
}