
#include "Rendering/Shader.hpp"
#include <Rendering/OpenGL/OpenGL_Shader.hpp>

#include <glad.h>

namespace Dimensional {

static std::string shaderTypeToString(ShaderType type)
{
    switch (type) {
    case NONE:
        return "NONE";
    case COMPUTE:
        return "COMPUTE";
    case VERTEX:
        return "VERTEX";
    case FRAGMENT:
        return "FRAGMENT";
    case GEOMETRY:
        return "GEOMETRY";
        break;
    }
}

void OpenGLShader::link(const std::vector<u32> programs)
{
    int result;
    char infoLog[512];

    m_GLID = glCreateProgram();

    for (u32 prog : programs) {
        glAttachShader(m_GLID, prog);
    }
    glLinkProgram(m_GLID);

    glGetProgramiv(m_GLID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(m_GLID, 512, NULL, infoLog);
        DM_CORE_ERROR("ERROR LINKING SHADER: {0}", infoLog);
    }

    for (u32 prog : programs) {
        glDeleteShader(prog);
    }
};

static u32 compile(const char* shaderProg, ShaderType type, const std::string& name)
{
    u32 shader;
    int result;
    char infoLog[2048];

    switch (type) {
    case (COMPUTE):
        shader = glCreateShader(GL_COMPUTE_SHADER);
        break;
    case (VERTEX):
        shader = glCreateShader(GL_VERTEX_SHADER);
        break;
    case (FRAGMENT):
        shader = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    case (GEOMETRY):
        shader = glCreateShader(GL_GEOMETRY_SHADER);
        break;
    default:
        DM_CORE_ERROR("Unkown ShaderType attempted to compile.");
        break;
    }

    glShaderSource(shader, 1, &shaderProg, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(shader, 2048, NULL, infoLog);
        DM_CORE_WARN("{0} {1} SHADER COMPILATION_FAILED {2}\n", name, shaderTypeToString(type), infoLog);
    };
    return shader;
};

OpenGLShader OpenGLShader::Create(const ShaderData& data)
{

    OpenGLShader shader;

    std::vector<u32> programs;
    for (auto [type, source] : data.programs) {
        programs.push_back(compile(source.c_str(), type, data.name));
    }
    shader.link(programs);

    return shader;
}

void OpenGLShader::Bind()
{
    glUseProgram(m_GLID);
}

void OpenGLShader::UnBind()
{
    glUseProgram(0);
}

void OpenGLShader::Destroy()
{
}
}
