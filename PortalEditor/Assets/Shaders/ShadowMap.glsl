
##VERTEXSHADER
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(aPos, 1.0);
}

##GEOMETRY
#version 450 core

#define MAX_DIRECTIONAL_LIGHTS 256

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

struct DirLight {
    vec4 direction;
    vec4 color;
    mat4 projection;
};
layout(std140, binding = 2) uniform DLightBlock {
    DirLight uDirLight[MAX_DIRECTIONAL_LIGHTS];
    uint uNumDirLights;
};

out vec4 FragPosLightSpace;

void main() {
    for (int i = 0; i < 2; i++) {
        gl_Layer = i;
        for (int j = 0; j < 3; j++) {
            FragPosLightSpace = uDirLight[i].projection * gl_in[j].gl_Position;
            gl_Position = FragPosLightSpace;
            EmitVertex();
        }
        EndPrimitive();
    }
}

##FRAGSHADER
#version 450 core

// layout(location = 0) in float fragDepth;

void main()
{
    // gl_FragDepth = fragDepth;
}
