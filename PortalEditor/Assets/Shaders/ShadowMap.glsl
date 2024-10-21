##VERTEXSHADER
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

struct DirLight {
    vec4 direction;
    vec4 color;
    mat4 projection;
};
layout(std140, binding = 2) uniform DLightBlock {
    DirLight uDirLight;
};

uniform mat4 model;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = uDirLight.projection * worldPos;
}

##FRAGSHADER
#version 450 core

layout(location = 0) out vec4 FragColor;

void main()
{
}

