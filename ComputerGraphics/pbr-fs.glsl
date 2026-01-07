/*
 * Fragment Shader for Physically Based Rendering Lighting and Image Base Lighting 
 */
#version 330 core


/********************** Vertex Shader Inputs **********************/
in vec3 exPosition;	   // Fragment Position
in vec2 exTexcoord;
in vec3 exNormal;	
in vec3 exTangent; 	
in vec3 exBitTangent;  
/******************************************************************/




/********************* Fragment Shader Outputs ********************/
out vec4 FragmentColor;
/******************************************************************/





/********************* Uniform Inputs *****************************/
//uniform samplerCube cubeMapSampler;
uniform sampler2D diffSampler;              // Albedo
uniform sampler2D normalSampler;
uniform sampler2D roughSampler;             // Roughness
uniform sampler2D metalSampler;
uniform samplerCube irradianceSampler;      // For IBL diffuse
uniform samplerCube prefilteredEnvSampler;  // For IBL specular
uniform sampler2D brdfLUTSampler;           // For IBL specular
uniform vec3 CameraPosition;                // cameraPosition
/******************************************************************/



/************************ Point Lights ****************************/
#define NUM_LIGHTS 4
vec3 lightPositions[NUM_LIGHTS] = vec3[](
    vec3(5.0, 5.0, 5.0),
    vec3(-5.0, 5.0, 5.0),
    vec3(5.0, 5.0, -5.0),
    vec3(-5.0, 5.0, -5.0)
);

vec3 lightColors[NUM_LIGHTS] = vec3[](
    vec3(00.0, 00.0, 00.0),  // White
    vec3(00.0, 00.0, 00.0),      // Red
    vec3(00.0, 00.0, 300.0),      // Green
    vec3(00.0, 00.0, 00.0)       // Blue
);
/*****************************************************************/




/********************* PBR and Helper Variables *******************/
float PI = 3.14159265359;
bool normalMapping = true;
mat3 TBN;

vec4 albedo;    
float metalness;
float roughness;
vec3 emissivity = vec3(0.0, 0.0, 0.0); // To add a sampler to sample from these textures

vec3 F0; // Base Reflectivity
vec3 N;
vec3 V;
vec3 R;  // Reflection Vector
vec3 F;  // Fresnel
vec3 L_direct; 
vec3 L_point;
/******************************************************************/







/********************* PBR Helper Functions ***********************/
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

float calcAttenuation(vec3 fragPos, vec3 lightPos) {
    vec3 lightVec = lightPos - fragPos;
    float distance = length(lightVec);
    return 1.0 / (distance * distance);
}
/******************************************************************/






/********************* PBR Functions ******************************/

/* 
 * Trowbridge-Reitz GGX Normal Distribution Function
 *
 * Param alpha: Surface roughness parameter.
 * Param N: Fragment normal vector.
 * Param H: Halfway vector between view and light directions.
 *
 * Statistically approximates the relative surface area of microfacets
 * exactly aligned with the halfway vector H.
 */
float NDF_TrowbridgeReitzGGX(float roughness, vec3 N, vec3 H) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);

    float num = a2;
    float denum = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
    denum = PI * denum * denum;

    return num / denum;
}

/* 
 * Schlick-GGX Geometry Function
 *
 * Param NdotV: Dot product of normal and view vector.
 * Param K: Geometry parameter derived from roughness.
 *
 * The Schlick-GGX function approximates the relative surface area 
 * that is obstructed by microfacets due to geometry obstruction (view angle obstruction)
 * and geometry shadowing (light direction obstruction).
 */
float GEOM_schlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float K = (r * r) / 8.0;

    float num = NdotV;
    float denum = NdotV * (1.0 - K) + K;

    return num / denum;
}

float GEOM_smith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0); // Geometry obstruction: When microfacets obstruct the view
    float NdotL = max(dot(N, L), 0.0); // Geometry shadowing: When surface geometry obstructs light
    float ggx1 = GEOM_schlickGGX(NdotV, roughness);
    float ggx2 = GEOM_schlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

/*
 * Fresnel Schilck Approximation Function 
 * 
 * Param NdotH: Dot product between surface normal and halfway vector.
 * Param F0: Material base reflectance 
 *
 * Calculates the ratio of light that gets reflected on a surface.
 */
vec3 F_fresnelSchlick(float HdotV, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - HdotV, 0.0, 1.0), 5.0);
}

vec3 F_fresnelSchlickRoughness(float NdotV, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - NdotV, 0.0, 1.0), 5.0);
}  


/*
 * Cook-Torrance BRDF Function
 *
 * Param F: Fresnel term.
 * Param NDF: Normal Distribution Function term.
 * Param G: Geometry Function term.
 * Param L: Light direction vector.
 * Param V: View direction vector.
 * Param N: Surface normal vector.
 *
 * Combines the Fresnel, Normal Distribution, and Geometry functions
 * to compute the specular reflection component of the BRDF.
 */
vec3 BRDF_cookTorrance(vec3 F, float NDF, float G, vec3 L, vec3 V, vec3 N) {
    vec3 num = NDF * F * G;
    float denum = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; 
    return num / denum;
}
/******************************************************************/

vec3 PBR_point() {
    vec3 LightOut = vec3(0.0);
    for (int i = 0; i < NUM_LIGHTS; i++) {
        vec3 L = normalize(lightPositions[i] - exPosition);     
        vec3 H = normalize(V + L);
       
        float attenuation = calcAttenuation(exPosition, lightPositions[i]);
        vec3 radiance = lightColors[i] * attenuation;

        vec3 F = F_fresnelSchlick(max(dot(H, V), 0.0), F0);
        float NDF = NDF_TrowbridgeReitzGGX(roughness, N, H);
        float G = GEOM_smith(N, V, L, roughness);
        vec3 specular = BRDF_cookTorrance(F, NDF, G, L, V, N);
        
        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metalness);

        float NdotL = max(dot(N, L), 0.0);
        LightOut += (kD * (albedo.rgb / PI) + specular) * radiance * NdotL;
    }

    return LightOut;
}


void main(void)
{   
    TBN = buildTBN();

    // PBR Materials
    albedo = texture(diffSampler, exTexcoord);      
    roughness = texture(roughSampler, exTexcoord).r;
    metalness = texture(metalSampler, exTexcoord).r;
    F0 = mix(vec3(0.04), albedo.rgb, metalness); // Interpolate based on metalness values (metal = 1.0)

    // test
    // FragmentColor = vec4(vec3(roughness), 1.0); return;
//    FragmentColor = vec4(vec3(metalness), 1.0); return;

    // PBR Vectors
    N = getNormals();
    V = normalize(CameraPosition - exPosition);
    R = reflect(-V, N);

    F = F_fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilteredEnvSampler, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUTSampler, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    // PBR Lighting
    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metalness);

    vec3 irradiance = texture(irradianceSampler, N).rgb;
    vec3 diffuse = irradiance * albedo.rgb;
    vec3 ambient = (kD * diffuse + specular);  // * AO <- we need to add this 

    vec3 color = ambient + PBR_point(); 
   
    FragmentColor = vec4(color, 1.0);
}
