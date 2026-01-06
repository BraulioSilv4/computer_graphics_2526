#version 330 core

in vec3 exPosition;	   // Fragment Position
in vec2 exTexcoord;
in vec3 exNormal;	
in vec3 exTangent; 	
in vec3 exBitTangent;  

out vec4 FragmentColor;

uniform samplerCube cubeMapSampler;
uniform sampler2D diffSampler; // Albedo
uniform sampler2D normalSampler;
uniform sampler2D roughSampler; // Roughness
uniform sampler2D metalSampler;
uniform vec3 CameraPosition; // cameraPosition
uniform vec4 LightColor;    // lightColor
uniform vec3 LightPosition; // lightPosition
vec3 emissivity = vec3(0.0, 0.0, 0.0); // To add a sampler to sample from these textures

mat3 TBN;
bool normalMapping = true;
float PI = 3.1415;

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
} // Fragment Normal

// PBR Materials
vec4 albedo;    
float roughness;
float metalness;
vec3 F0; // Base Reflectivity

// PBR Vectors
vec3 N;
vec3 V;
vec3 L_direct; 
vec3 L_point;
    

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

/* Distribution Functions */
float GGX_Trowbridge_Reitz(float alpha, vec3 N, vec3 H) {
    float numerator = pow(alpha, 2.0);

    float NdotH = max(dot(N, H), 0.0);
    float denominator = PI * pow(pow(NdotH, 2.0) * (pow(alpha, 2.0) - 1.0) + 1.0, 2.0);
    denominator = max(denominator, 0.000001);
    
    return numerator / denominator;
}

// X is either the view or light vector
float schlick_beckmann(float alpha, vec3 N, vec3 X) {
    float numerator = max(dot(N, X), 0.0);

    float k = alpha / 2.0;
    float denominator = max(dot(N, X), 0.0) * (1.0 - k) + k;
    denominator = max(denominator, 0.000001);

    return numerator / denominator;
}


float smith(float alpha, vec3 N, vec3 V, vec3 L) {
    return schlick_beckmann(alpha, N, V) * schlick_beckmann(alpha, N, L);
}

/* 
    F0: Base Reflectivity
*/
vec3 fresnel_schlick(vec3 F0, vec3 V, vec3 H) {
    return F0 + (vec3(1.0) - F0) * pow(1 - max(dot(V, H), 0.0), 5.0);
}


vec3 PBR_PointLight() {
    vec3 L = L_point;
    vec3 H = normalize(V + L);

    vec3 Ks = fresnel_schlick(F0, V, H);
    vec3 Kd = vec3(1.0) - Ks * (1.0 - metalness);

    float alpha = roughness * roughness;

    vec3 lambert = albedo.rgb / PI;
    float D = GGX_Trowbridge_Reitz(alpha, N, H);
    float G = smith(alpha, N, V, L_point);
    vec3 F = Ks;

    vec3 cookTorranceNumerator = D * G * F;
    float cookTorranceDenominator = 4.0 * max(dot(V, N), 0.0) * max(dot(L_point, N), 0.0);
    cookTorranceDenominator = max(cookTorranceDenominator, 0.000001);
    vec3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

    float distance = length(LightPosition - exPosition);
    float a = 0.00;
    float b = 0.00;
//    float attenuation = 1.0 / (a * distance * distance + b * distance + 1.0);
    float attenuation = 1.0 / (distance * distance + 1.0);
    

    vec3 BRDF = Kd * lambert + cookTorrance;
    vec3 outgoingLight = BRDF * LightColor.rgb * max(dot(N, L_point), 0.0) * attenuation;

    return outgoingLight;
}

vec3 PBR_DirectLight() {
    vec3 L = L_direct;
    vec3 H = normalize(V + L);

    /* Specular Component (Light Reflected) */
    vec3 Ks = fresnel_schlick(F0, V, H);

    /* Diffuse Component (Light Absorbed) */
    vec3 Kd = (vec3(1.0) - Ks) * (1.0 - metalness); 

    float alpha = roughness * roughness;

    vec3 lambert = albedo.rgb / PI;
    float D = GGX_Trowbridge_Reitz(alpha, N, H);
    float G = smith(alpha, N, V, L);
    vec3 F = Ks;

    vec3 cookTorranceNumerator = D * G * F;
    float cookTorranceDenominator = 4.0 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0);
    cookTorranceDenominator = max(cookTorranceDenominator, 0.000001);
    vec3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

    vec3 BRDF = Kd * lambert + cookTorrance;
    vec3 outgoingLight = BRDF * LightColor.rgb * max(dot(N, L), 0.0);

    return outgoingLight;
}

void main(void)
{   
    TBN = buildTBN();

    // PBR Materials
    albedo = texture(diffSampler, exTexcoord);    
    roughness = texture(roughSampler, exTexcoord).r;
    metalness = texture(metalSampler, exTexcoord).r;
    F0 = mix(vec3(0.04), albedo.rgb, metalness); // Interpolate based on metalness values (metal = 1.0)

    // PBR Vectors
    N = getNormals();
    V = normalize(CameraPosition - exPosition);
    L_direct = normalize(vec3(0.0, 1.0, 0.0)); 
    L_point = normalize(LightPosition - exPosition);
    
    // PBR Lighting
    vec3 ambient = vec3(0.03) * albedo.rgb;
    vec3 lighting = ambient  + PBR_PointLight();
    
    FragmentColor = vec4(lighting + emissivity, 1.0);
}
