#version 450 core

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(binding = 0) uniform samplerCube uMap;

layout(binding = 1, rgba16f) uniform imageCube uOutputCube;

const float PI = 3.14159265359;

void main() {
    ivec3 texelCoord = ivec3(gl_GlobalInvocationID.xy, gl_GlobalInvocationID.z);

    vec2 st = gl_GlobalInvocationID.xy / vec2(imageSize(uOutputCube));
    vec2 uv = 2.0 * vec2(st.x, 1.0 - st.y) - vec2(1.0);

    vec3 localPosition;
    switch (gl_GlobalInvocationID.z) {
        case (0):
        localPosition = vec3(1.0, uv.y, -uv.x);
        break;
        case (1):
        localPosition = vec3(-1.0, uv.y, uv.x);
        break;
        case (2):
        localPosition = vec3(uv.x, 1.0, -uv.y);
        break;
        case (3):
        localPosition = vec3(uv.x, -1.0, uv.y);
        break;
        case (4):
        localPosition = vec3(uv.x, uv.y, 1.0);
        break;
        case (5):
        localPosition = vec3(-uv.x, uv.y, -1.0);
        break;
    }

    vec3 normal = normalize(localPosition);
    vec3 irradiance = vec3(0.0);

    vec3 up = abs(normal.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    // Spherical sampling resolution
    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
            // Spherical to cartesian conversion in tangent space
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            // Sample the environment map
            irradiance += texture(uMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    imageStore(uOutputCube, texelCoord, vec4(irradiance, 1.0));
}
