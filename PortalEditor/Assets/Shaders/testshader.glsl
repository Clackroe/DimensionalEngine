##VERTEXSHADER
#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aCol;
layout(location = 2) in vec2 aUV;

out vec3 col;
out vec2 uv;

void main()
{
    col = aCol;
    uv = aUV;
    gl_Position = vec4(aPos, 1);
}

##FRAGSHADER
#version 450 core

out vec4 FragColor;
in vec3 col;
in vec2 uv;
layout(binding = 1) uniform sampler2D tex;

void main()
{
    FragColor = vec4(texture(tex, uv).rgb, 1);
}
