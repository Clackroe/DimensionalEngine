#version 450 core

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(binding = 0) uniform sampler2D uEquiRectMap;
layout(binding = 1, rgba16f) restrict writeonly uniform imageCube cubeMap;

const float PI = 3.141592;
const float PIPI = 2 * PI;
vec3 getSampleDir()
{
    vec2 st = gl_GlobalInvocationID.xy / vec2(imageSize(cubeMap));
    vec2 uv = 2.0 * vec2(st.x, 1.0 - st.y) - vec2(1.0);

    vec3 ret;
    switch (gl_GlobalInvocationID.z) {
        case (0):
        ret = vec3(1.0, uv.y, -uv.x);
        break;
        case (1):
        ret = vec3(-1.0, uv.y, uv.x);
        break;
        case (2):
        ret = vec3(uv.x, 1.0, -uv.y);
        break;
        case (3):
        ret = vec3(uv.x, -1.0, uv.y);
        break;
        case (4):
        ret = vec3(uv.x, uv.y, 1.0);
        break;
        case (5):
        ret = vec3(-uv.x, uv.y, -1.0);
        break;
    }
    return normalize(ret);
}

void main(void)
{
    vec3 v = getSampleDir();

    float phi = atan(v.z, v.x);
    float theta = acos(v.y);

    vec4 color = texture(uEquiRectMap, vec2(phi / PIPI, 1.0 - (theta / PI)));

    imageStore(cubeMap, ivec3(gl_GlobalInvocationID), color);
}
