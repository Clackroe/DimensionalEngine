##VERTEXSHADER
#version 330 core
layout(location = 0) in vec3 aPosition;

out vec3 localPosition;

uniform mat4 uProj;
uniform mat4 uView;

void main() {
    localPosition = aPosition;
    gl_Position = uProj * uView * vec4(aPosition, 1.0);
    // gl_Position = viewProj *vec4(aPosition, 1.0);
}

##FRAGSHADER
#version 330 core

out vec4 FragColor;
in vec3 localPosition;

uniform sampler2D uEquirectMap;

const vec2 invAtan = vec2(atan(0.1591, 0.3183));
vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
void main() {
    vec2 uv = SampleSphericalMap(normalize(localPosition));
    vec3 color = texture(uEquirectMap, uv).rgb;
    FragColor = vec4(color, 1.0);
}
