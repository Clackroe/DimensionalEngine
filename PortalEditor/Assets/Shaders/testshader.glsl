##VERTEXSHADER
#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aCol;
layout(location = 2) in vec2 aUV;

out vec3 col;
out vec2 uv;

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

layout(std140, binding = 1) uniform Temp {
    mat4 model;
};

void main()
{
    vec3 WorldPos = vec3(model * vec4(aPos, 1.0));

    col = aCol;
    uv = aUV;
    gl_Position = viewProj * vec4(WorldPos, 1.0);
}

##FRAGSHADER
#version 450 core

layout(std140, binding = 2) uniform Color {
    vec3 color;
};

layout(std430, binding = 3) buffer Color2 {
    vec3 color2;
};

out vec4 FragColor;
in vec3 col;
in vec2 uv;
layout(binding = 1) uniform sampler2D tex;

void main()
{
    FragColor = vec4(texture(tex, uv).rgb * color2, 1);
}
