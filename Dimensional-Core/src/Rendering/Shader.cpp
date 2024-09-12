#include "Log/log.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <Rendering/Shader.hpp>

#include <glad.h>

namespace Dimensional {
Shader::Shader(const std::string& path, enum ShaderType type)
    : Asset(path, AssetType::ShaderType)
{
    m_Type = type;

    std::string vertexSourceCode;
    std::string fragmentSourceCode;

    std::ifstream file;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path);
        std::stringstream stream;

        stream << file.rdbuf();

        file.close();

        std::string line;
        bool isVertex = false;
        bool isFrag = false;
        while (std::getline(stream, line)) {
            if (line.empty()) {
                continue;
            }
            if (line.find("##VERTEXSHADER") != std::string::npos) {
                isVertex = true;
                isFrag = false;
                continue;
            }
            if (line.find("##FRAGSHADER") != std::string::npos) {
                isVertex = false;
                isFrag = true;
                continue;
            }
            if (isVertex) {
                vertexSourceCode += (line + "\n");
                continue;
            }
            if (isFrag) {
                fragmentSourceCode += (line + "\n");
                continue;
            }
            DM_CORE_WARN("WARNING: POSSIBLE MISFORMATTED SHADERFILE")
        }

    } catch (std::ifstream::failure err) {
        DM_CORE_ERROR("ERROR WITH READING THE SHADER FILES: {0}", err.what());
    }
    const char* vShaderProg = vertexSourceCode.c_str();
    const char* fShaderProg = fragmentSourceCode.c_str();
    u32 vertexID = compile(vShaderProg, VERTEX);
    u32 fragID = compile(fShaderProg, FRAGMENT);
    link({ vertexID, fragID });
}

Shader::Shader(const std::string& vertexPath, const std::string& fragPath)
    : Asset(vertexPath + fragPath, AssetType::ShaderType)
{
    m_Type = RENDER;
    std::string vertexSourceCode;
    std::string fragmentSourceCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexSourceCode = vShaderStream.str();
        fragmentSourceCode = fShaderStream.str();
    } catch (std::ifstream::failure err) {
        DM_CORE_ERROR("ERROR WITH READING THE SHADER FILES: {0}", err.what());
    }
    const char* vShaderProg = vertexSourceCode.c_str();
    const char* fShaderProg = fragmentSourceCode.c_str();
    u32 vertexID = compile(vShaderProg, VERTEX);
    u32 fragID = compile(fShaderProg, FRAGMENT);
    link({ vertexID, fragID });
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setMat4(const std::string& name, glm::mat4 value)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setMat3(const std::string& name, glm::mat3 value)
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::link(std::vector<u32> programs)
{
    int result;
    char infoLog[512];

    ID = glCreateProgram();
    for (u32& prog : programs) {
        glAttachShader(ID, prog);
    }
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        DM_CORE_ERROR("ERROR LINKING SHADER: {0}", infoLog);
    }

    for (u32& prog : programs) {
        glDeleteShader(prog);
    }
}

void Shader::dispatchCompute(u32 width, u32 height, u32 depth)
{
    if (m_Type != COMPUTE) {
        DM_CORE_WARN("Attempted to dispatch a compute shader on a non-compute shader");
        return;
    }
    use();
    glDispatchCompute(width, height, depth);
}

u32 Shader::compile(const char* shaderProg, enum ShaderType type)
{
    u32 shader;
    int result;
    char infoLog[512];

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
    default:
        DM_CORE_ERROR("Unkown ShaderType attempted to compule.");
        break;
    }

    glShaderSource(shader, 1, &shaderProg, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        DM_CORE_WARN("VERTEX SHADER COMPILATION_FAILED {0}\n", infoLog);
    };
    return shader;
}

}
