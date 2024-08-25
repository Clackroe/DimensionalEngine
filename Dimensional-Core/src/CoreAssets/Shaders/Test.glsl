##VERTEX
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 viewProj;
uniform mat4 model;

void main()
{
    gl_Position = viewProj * model * vec4(aPos, 1.0);
    // ourColor = aColor;
    // TexCoord = aTexCoord;
}

##FRAG

#version 430 core
out vec4 FragColor;

// in vec3 ourColor;
// in vec2 TexCoord;

// uniform sampler2D ourTexture;

void main()
{
    FragColor = vec4(1.0);
}
