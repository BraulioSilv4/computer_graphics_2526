#version 330 core

in vec3 exPosition;	
in vec2 exTexcoord;
in vec3 exNormal;	
in vec3 exTangent; 	
in vec3 exBitTangent;  

out vec4 FragmentColor;

uniform sampler2D diffSampler;
uniform sampler2D normalSampler;
uniform sampler2D roughSampler;
uniform sampler2D metalSampler;
uniform samplerCube cubeMapSampler;

uniform vec3 CameraPosition;
uniform vec4 LightColor;
uniform vec3 LightPosition;

mat3 TBN;
bool normalMapping = true;

mat3 buildTBN() {
    vec3 N = normalize(exNormal);
    vec3 T = normalize(exTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    return mat3(T, B, N);
}


vec3 getNormals() {
    if(normalMapping) {
        vec3 normalMap = texture(normalSampler, exTexcoord).xyz * 2.0 - 1.0;
        return normalize(TBN * normalMap);
    } else {
        return normalize(exNormal);
    }
}

vec4 blinnPhongPointLight() {
    vec3 lightVec = LightPosition - exPosition;
    float distance = length(lightVec);
    float a = 0.00;
    float b = 0.00;
    float attenuation = 1.0 / (a * distance * distance + b * distance + 1.0);

    // Normal Mapping
    vec3 normal = getNormals();
    
    // Diffuse
    vec3 lightDirection = normalize(lightVec);
    float diffuse = max(dot(normal, lightDirection), 0.0);

    // Specular
    float specularLight = 0.5;
    vec3 viewDirection = normalize(CameraPosition - exPosition);
    vec3 halfVector = normalize(lightDirection + viewDirection);
    float specAmount = pow(max(dot(normal, halfVector), 0.0), 16.0);
    float specular = specAmount * specularLight;

    return LightColor * ((diffuse + specular) * attenuation);
}

vec4 blinnPhongDirectLight(vec3 direction) {    
    // Normal Mapping
    vec3 normal = getNormals();
    
    // Diffuse 
    vec3 lightDirection = normalize(direction);
    float diffuse = max(dot(normal, lightDirection), 0.0);

    // Specular
    float specularLight = 0.5f;
    vec3 viewDirection = normalize(CameraPosition - exPosition);
    vec3 halfVector = normalize(lightDirection + viewDirection);
    float specAmount = pow(max(dot(normal, halfVector), 0.0), 16);
    float specular = specAmount * specularLight;

    return LightColor * ((diffuse + specular));
}

vec4 blinnPhongLighting() {
    // Ambient
    float ambient = 0.2;
    vec4 ambientLight = ambient * LightColor;

    return (blinnPhongPointLight() + blinnPhongDirectLight(vec3(1.0, 1.0, 0.0)) + ambientLight);
}



void main(void)
{   
    TBN = buildTBN();
    vec4 albedo = texture(diffSampler, exTexcoord);    
    
    FragmentColor = albedo * blinnPhongLighting();
}
