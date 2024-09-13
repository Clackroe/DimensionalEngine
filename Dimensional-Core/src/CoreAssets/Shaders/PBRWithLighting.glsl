##VERTEXSHADER
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

struct Vertex {
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
    mat3 TBN;
};

layout(location = 0) out Vertex vOutput;

uniform mat4 viewProj;
uniform mat4 model;

void main()
{
    vOutput.TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vOutput.WorldPos = vec3(model * vec4(aPos, 1.0));
    vOutput.Normal = normalize(normalMatrix * aNormal);

    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(aBiTangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    T = normalize(cross(N, B));
    vOutput.TBN = mat3(T, B, N);

    gl_Position = viewProj * vec4(vOutput.WorldPos, 1.0);
}

##FRAGSHADER
#version 430 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 attachment1;

struct Vertex {
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
    mat3 TBN;
};

layout(location = 0) in Vertex vInput;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// lights

struct Light {
    vec3 position;
    vec3 direction;
    vec3 color;

    float intensity;
    float cutOff; // Spotlight cutoff angle
    float outerCutOff; // Spotlight outer cutoff angle

    // Attenuation parameters
    float constant;
    float linear;
    float quadratic;
};
uniform Light uPointLights[50];
uniform Light uSpotLights[50];
uniform int uNumPointLights;
uniform int uNumSpotLights;

uniform samplerCube uIrradianceMap;
uniform sampler2D uBRDFLut;
uniform samplerCube uIBLMap;

uniform bool uShouldUseNormalMap;

uniform vec3 uCameraPosition;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
vec3 getNormalFromMap()
{
    if (!uShouldUseNormalMap) {
        return normalize(vInput.Normal);
    }
    vec3 tangentNormal = texture(normalMap, vInput.TexCoords).xyz * 2.0 - 1.0;

    return normalize(vInput.TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom + 1e-5;

    return a2 / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickWithRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
void main()
{
    vec3 albedo = pow(texture(albedoMap, vInput.TexCoords).rgb, vec3(2.2));
    float metallic = texture(metallicMap, vInput.TexCoords).r;
    float roughness = texture(roughnessMap, vInput.TexCoords).r;
    float ao = texture(aoMap, vInput.TexCoords).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(uCameraPosition - vInput.WorldPos);
    vec3 R = normalize(reflect(-V, N));

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    const float epsilon = 0.000001;
    // Point lights contribution
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < uNumPointLights; ++i)
    {
        vec3 lightDirection = uPointLights[i].position - vInput.WorldPos;
        float distance = length(lightDirection);
        vec3 L = normalize(lightDirection); // Normalized light direction
        vec3 H = normalize(V + L);

        float attenuation = 1.0 / (uPointLights[i].constant +
                    uPointLights[i].linear * distance +
                    uPointLights[i].quadratic * (distance * distance));

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + epsilon; // Prevent division by zero
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

        float NdotL = max(dot(N, L), 0.0);

        Lo += ((kD * albedo / PI) + specular) * (uPointLights[i].color * uPointLights[i].intensity) * NdotL * attenuation;
    }

    // Spotlight contribution
    for (int i = 0; i < uNumSpotLights; ++i)
    {
        vec3 L = normalize(uSpotLights[i].position - vInput.WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(uSpotLights[i].position - vInput.WorldPos);
        float attenuation = 1.0 / (uSpotLights[i].constant + uSpotLights[i].linear * distance + uSpotLights[i].quadratic * (distance * distance));

        float theta = dot(L, normalize(-uSpotLights[i].direction));
        float epsilon = uSpotLights[i].cutOff - uSpotLights[i].outerCutOff;
        float intensity = clamp((theta - uSpotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + epsilon;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

        float NdotL = max(dot(N, L), 0.0);
        vec3 diffuse = kD * albedo / PI;
        vec3 radiance = (diffuse + specular) * (uSpotLights[i].color * uSpotLights[i].intensity) * NdotL;
        Lo += radiance * attenuation * intensity;
    }

    // Calculate ambient lighting from scene IBLMaps
    vec3 F = fresnelSchlickWithRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(uIrradianceMap, N).rgb;
    vec3 diff = irradiance * albedo;

    const float MAX_REF_LOD = 4.0;
    vec3 prefilterColor = textureLod(uIBLMap, R, roughness * MAX_REF_LOD).rgb;
    vec2 envBRDF = texture(uBRDFLut, vec2(clamp(dot(N, V), 0.0, 1.0), roughness)).rg;
    vec3 specular = prefilterColor * (F); // max(envBRDF.r, 0.01) + max(envBRDF.g, 0.01));

    vec3 ambient = (kD * diff + specular) * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // // gamma correct
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
    // FragColor = vec4(envBRDF, 0.0, 1.0);
    // FragColor = vec4(N * 0.5 + 0.5, 1.0);
    // attachment1 = vec4(N, 1.0);
    // FragColor = vec4(N, 1.0);
    // FragColor = vec4(vInput.Tangent, 1.0);
    // FragColor = vec4(uPointLights[0].position, 1.0);
}
