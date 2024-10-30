##VERTEXSHADER
#version 450 core

#define MAX_POINTLIGHTS 256
#define MAX_DIRECTIONAL_LIGHTS 256

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

layout(std140, binding = 0) uniform CameraBlock {
    mat4 viewProj;
    vec3 uCameraPosition;

    mat4 view;
    mat4 proj;
};

struct DirLight {
    vec4 direction;
    vec4 color;
    mat4 projection;
};
layout(std140, binding = 2) uniform DLightBlock {
    DirLight uDirLight[MAX_DIRECTIONAL_LIGHTS];
    uint uNumDirLights;
};

struct Vertex {
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
    mat3 TBN;
};

layout(location = 0) out Vertex vOutput;

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

    // vOutput.posDirLightSpace = uDirLight.projection * vec4(vOutput.WorldPos, 1.0);

    gl_Position = viewProj * vec4(vOutput.WorldPos, 1.0);
    // gl_Position = vec4(aPos, 1.0);
}

##FRAGSHADER
#version 450 core

#define MAX_POINTLIGHTS 256
#define MAX_DIRECTIONAL_LIGHTS  256

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 attachment1;

struct Vertex {
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
    mat3 TBN;
    // vec4 posDirLightSpace;
};

layout(location = 0) in Vertex vInput;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform vec3 uColorValue;
uniform float uMetalnessMult;
uniform float uRoughnessMult;

uniform bool uUseRoughnessMap;
uniform bool uUseMetalMap;

layout(std140, binding = 0) uniform CameraBlock {
    mat4 viewProj;
    vec3 uCameraPosition;

    mat4 view;
    mat4 proj;
};

// lights
struct DirLight {
    vec4 direction;
    vec4 color;
    mat4 projection;
};

layout(std140, binding = 2) uniform DLightBlock {
    DirLight uDirLight[MAX_DIRECTIONAL_LIGHTS];
    uint uNumDirLights;
};
layout(binding = 6) uniform sampler2DArray uDirLightShadowMaps;

struct Light {
    vec3 position;
    vec3 direction;
    vec4 color; // vec3 color, intensity;
    vec4 lightParams; // Combined (cutOff, outerCutoff, Radius, type[0 plight, 1 spotLight]);
};
layout(std140, binding = 1) uniform PLightBlock {
    Light uPointLights[MAX_POINTLIGHTS];
    uint uNumPointLights;
};

layout(binding = 9) uniform samplerCube uIrradianceMap;
layout(binding = 7) uniform sampler2D uBRDFLut;
layout(binding = 8) uniform samplerCube uIBLMap;

uniform bool uShouldUseNormalMap;

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

float shadowCalculation(vec4 lightSpace, int layer) {
    vec3 projected = lightSpace.xyz / lightSpace.w;
    projected = projected * 0.5 + 0.5;
    float closest = texture(uDirLightShadowMaps, vec3(projected.xy, layer)).r;
    float current = projected.z;
    float bias = 0.005;
    float shadow = current - bias < closest ? 1.0 : 0.0;
    return shadow;
}

// Light Falloff/Attentuation based on paper by epic games: https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
float lightAttenuation(float distance, float radius) {
    float a = distance / radius;
    float b = clamp(1 - pow(a, 4), 0, 1);
    float n = pow(b, 2);
    return n / (pow(distance, 2) + 1);
}
void main()
{
    vec4 albedoSample = texture(albedoMap, vInput.TexCoords) * vec4(uColorValue, 1.0f);
    vec3 albedo = pow(albedoSample.rgb, vec3(2.2)); // Gamma correction for albedo

    float metallic = 1.0;
    if (uUseMetalMap) {
        metallic = texture(metallicMap, vInput.TexCoords).r;
    }
    metallic *= max(uMetalnessMult, 0);

    float roughness = 1.0;
    if (uUseRoughnessMap) {
        roughness = texture(roughnessMap, vInput.TexCoords).r;
    }
    roughness *= uRoughnessMult;
    roughness = clamp(roughness, 0.05, 0.95);

    float ao = texture(aoMap, vInput.TexCoords).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(uCameraPosition - vInput.WorldPos);
    vec3 R = normalize(reflect(-V, N));

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    const float epsilon = 0.000001;
    // Point lights contribution

    vec3 lightContribution = vec3(0.0);

    // TEMP DirLight

    for (int i = 0; i < uNumDirLights; i++)
    {
        vec3 L = normalize(uDirLight[i].direction.xyz);
        vec3 H = normalize(V + L);

        float NdotL = max(dot(N, L), 0.0);
        float shadow = shadowCalculation(uDirLight[i].projection * vec4(vInput.WorldPos, 1.0), i);

        vec3 radiance = shadow * uDirLight[i].color.rgb * uDirLight[i].color.a;

        float NDF = DistributionGGX(N, H, roughness);
        // float G = GeometrySmith(NDOTL, V, L, roughness);
        float G = GeometrySmith(N, V, L, roughness);

        // Maybe Try with the roughness function?
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + epsilon; // Prevent division by zero
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
        lightContribution += (kD * (albedo / PI) + specular) * radiance * NdotL;
    }

    //

    for (int i = 0; i < uNumPointLights; ++i) {
        Light light = uPointLights[i];
        vec3 lightPos = light.position.xyz;
        vec3 lightDirection = lightPos - vInput.WorldPos;
        vec3 lightColor = light.color.rgb;
        float lightIntensity = light.color.w;

        float innerCutoff = light.lightParams.x;
        float outerCutoff = light.lightParams.y;
        float radius = light.lightParams.z;

        uint type = uint(round(light.lightParams.w));

        float distance = length(lightDirection);
        vec3 L = normalize(lightDirection);
        vec3 H = normalize(V + L);

        float NDOTL = max(dot(N, L), 0.0);

        float attenuation;
        switch (type) {
            case 0: // PointLight Attenuation
            {
                attenuation = lightAttenuation(distance, radius);
                break;
            }
            case 1: // SpotLight Attenuation
            {
                float lDistSquared = dot(L, L);
                float radiusSquared = radius * radius;
                if (radiusSquared > lDistSquared) {
                    float theta = dot(L, normalize(-light.direction));
                    float epsilon = innerCutoff - outerCutoff;
                    float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
                    attenuation = intensity / lDistSquared;
                }
                else {
                    attenuation = 0.0;
                }
                break;
            }
        }
        //Directional LIght

        // Calculate Lighting with calculated light Prameters.
        vec3 lightContRadiance = lightColor * lightIntensity * attenuation;

        float NDF = DistributionGGX(N, H, roughness);
        // float G = GeometrySmith(NDOTL, V, L, roughness);
        float G = GeometrySmith(N, V, L, roughness);

        // Maybe Try with the roughness function?
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + epsilon; // Prevent division by zero
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
        lightContribution += (kD * (albedo / PI) + specular) * lightContRadiance * NDOTL;
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
    vec3 specular = prefilterColor * (F * max(envBRDF.r, 0.01) + max(envBRDF.g, 0.01));

    vec3 ambient = (kD * diff + specular) * ao;

    vec3 color = ambient + lightContribution;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // // gamma correct
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
    // FragColor = vec4(texture(uDepth, vInput.TexCoords).rgb, 1.0);
    // FragColor = vec4(shadow, 0.0, 0.0, 1.0);
    // FragColor = vec4(uPointLights[1].position, 1.0);
    // float v = float(uNumPointLights);
    // FragColor = vec4(v, 0, 0, 1.0);
}
