##VERTEXSHADER
#version 450 core

// layout(location = 0) in vec3 aPos;
// layout(location = 1) in vec3 aCol;
// layout(location = 2) in vec2 aUV;

out vec3 col;
out vec2 uv;

struct Vertex {
    vec3 Position;
    vec3 Normal;
    vec3 Tangent;
    vec3 BiTangent;
    vec2 TexCoords;
};

layout(binding = 8, std430) readonly buffer Vertices {
    Vertex vertices[];
};

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
    vec3 aPos = vertices[gl_VertexID].Position;
    vec2 aUV = vertices[gl_VertexID].TexCoords;
    vec3 WorldPos = vec3(model * vec4(aPos, 1.0));

    col = vec3(1, 1, 0);
    uv = aUV;
    gl_Position = viewProj * vec4(WorldPos, 1.0);
}

##FRAGSHADER
#version 450 core

layout(std140, binding = 2) uniform Color {
    vec3 color;
};

layout(std430, binding = 4) buffer Color2 {
    vec3 color2;
};

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 attachment1;
layout(location = 2) out vec4 attachment2;

in vec3 col;
in vec2 uv;
layout(binding = 1) uniform sampler2D tex;

void main()
{
    FragColor = vec4(texture(tex, uv).rgb, 1);
    attachment1 = vec4(1, 0, 0, 1);
    attachment2 = vec4(1, 1, 0, 1);
}
