#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 atexCoord;

uniform mat4 model;
uniform mat4 viewProj;

void main()
{
    gl_Position = viewProj * model * vec4(aPos, 1.0);
}
