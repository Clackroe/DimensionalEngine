#include "Log/log.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <Rendering/Shader.hpp>

#include <glad.h>

namespace Dimensional {

static std::string shaderTypeToString(ShaderType type)
{
    switch (type) {
    case NONE:
        return "NONE";
    case RENDER:
        return "RENDER";
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

Shader::Shader(const std::string& path, enum ShaderType type)
{
    m_Type = type;

    // TODO: Cleanup this function and factor out the file reading code
    if (m_Type == COMPUTE) {
        std::string computeSourceCode;
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            file.open(path);
            std::stringstream stream;

            stream << file.rdbuf();
            file.close();
            computeSourceCode = stream.str();
        } catch (std::ifstream::failure err) {
            DM_CORE_ERROR("ERROR WITH READING THE SHADER FILES: {0}", err.what());
        }
        u32 computeID = compile(computeSourceCode.c_str(), COMPUTE, std::filesystem::path(path));
        link({ computeID });
        return;
    }

    UMap<ShaderType, std::string> sourcesMap;
    ShaderType currentShader = NONE;

    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path);
        std::stringstream stream;

        stream << file.rdbuf();

        file.close();

        std::string line;
        while (std::getline(stream, line)) {
            if (line.empty()) {
                continue;
            }

            if (line.find("##VERTEXSHADER") != std::string::npos) {
                currentShader = VERTEX;
                sourcesMap[currentShader] = std::string();
                continue;
            }
            if (line.find("##FRAGSHADER") != std::string::npos) {
                currentShader = FRAGMENT;
                sourcesMap[currentShader] = std::string();
                continue;
            }
            if (line.find("##GEOMETRY") != std::string::npos) {
                currentShader = GEOMETRY;
                sourcesMap[currentShader] = std::string();
                continue;
            }
            if (currentShader != NONE) {
                sourcesMap[currentShader] += (line + "\n");
                continue;
            }
            DM_CORE_WARN("WARNING: POSSIBLE MISFORMATTED SHADERFILE")
        }

    } catch (std::ifstream::failure err) {
        DM_CORE_ERROR("ERROR WITH READING THE SHADER FILES: {0}", err.what());
    }
    std::vector<u32> ids;
    for (const auto& [type, source] : sourcesMap) {
        ids.push_back(compile(source.c_str(), type, std::filesystem::path(path)));
    }
    link(ids);
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
    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

u32 Shader::compile(const char* shaderProg, enum ShaderType type, const std::filesystem::path& path)
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
        DM_CORE_WARN("{0} {1} SHADER COMPILATION_FAILED {2}\n", path.stem().string(), shaderTypeToString(type), infoLog);
    };
    return shader;
}
}
