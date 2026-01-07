#version 330 core 

out vec4 FragColor;
in vec2 exTexcoords;

uniform sampler2D screenSampler;
uniform float gamma;
uniform float exposure;

vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}


void main() {
	vec3 hdrColor = texture(screenSampler, exTexcoords).rgb;

    hdrColor *= exposure;

    vec3 toneMapped = ACESFilm(hdrColor);
    
    //	vec3 toneMapped = vec3(1.0f) - exp(-fragment.rgb * exposure);
	//	color = color / (color + vec3(1.0)); // Reinhard tone mapping

	FragColor = vec4(pow(hdrColor, vec3(1.0f / gamma)), 1.0);
}

