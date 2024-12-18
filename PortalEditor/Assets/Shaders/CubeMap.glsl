##VERTEXSHADER
#version 430 core
layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform CameraBlock {
    mat4 viewProj;
    vec3 uCameraPosition;
    float aspectRatio;
    mat4 view;
    mat4 proj;
    float fov;
    float near;
    float far;
};

out vec3 localPos;

void main()
{
    localPos = aPos;

    mat4 rotView = mat4(mat3(view)); // remove translation from the view matrix
    vec4 clipPos = proj * rotView * vec4(localPos, 1.0);

    gl_Position = clipPos.xyww;
}

##FRAGSHADER
#version 430 core
out vec4 FragColor;

in vec3 localPos;

layout(binding = 8) uniform samplerCube environmentMap;
uniform float uLod;

void main()
{
    // vec3 envColor = texture(environmentMap, localPos).rgb;
    vec3 envColor = textureLod(environmentMap, localPos, uLod).rgb;

    // envColor = envColor / (envColor + vec3(1.0));
    // envColor = pow(envColor, vec3(1.0 / 2.2));

    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    FragColor = vec4(envColor, 1.0);
}
