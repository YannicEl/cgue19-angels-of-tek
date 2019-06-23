#version 430 core

// https://learnopengl.com/PBR/Theory

in VertexData {
    vec3 position_world;
    vec3 normal_world;
    vec2 uv;
} vert;

uniform vec3 cameraWorldPosition;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// lights
uniform vec3 lightPositions[11];
uniform vec3 lightColors[11];

uniform float prightness;

out vec4 FragColor;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, vert.uv).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(vert.position_world);
    vec3 Q2  = dFdy(vert.position_world);
    vec2 st1 = dFdx(vert.uv);
    vec2 st2 = dFdy(vert.uv);

    vec3 N   = normalize(vert.normal_world);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{		
    vec3 albedo     = pow(texture(albedoMap, vert.uv).rgb, vec3(2.2));
    float metallic  = texture(metallicMap, vert.uv).r;
    float roughness = texture(roughnessMap, vert.uv).r;
    float ao        = texture(aoMap, vert.uv).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(cameraWorldPosition - vert.position_world);

	// calculate reflectance
    vec3 F0 = vec3(0.01); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // calculate radiance for every light
        vec3 L = normalize(lightPositions[i] - vert.position_world);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - vert.position_world);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance Model
        float NDF = DistributionGGX(N, H, roughness);   
        float G = GeometrySmith(N, V, L, roughness);      
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 nominator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = nominator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        float NdotL = max(dot(N, L), 0.0);        

        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }   
    
    vec3 ambient = vec3(0.03) * albedo * ao * prightness;
    
	// Final Color
    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 
    FragColor = vec4(color, 1.0);
}