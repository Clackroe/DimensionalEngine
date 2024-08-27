#include "Log/log.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <Rendering/Shader.hpp>

#include <glad.h>

namespace Dimensional {
Shader::Shader(const std::string& path)
    : Asset(path)
{
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
            if (line.find("##VERTEX") != std::string::npos) {
                isVertex = true;
                isFrag = false;
                continue;
            }
            if (line.find("##FRAG") != std::string::npos) {
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

    unsigned int vShader, fShader;
    int result;
    char infoLog[512];

    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShaderProg, NULL);
    glCompileShader(vShader);

    glGetShaderiv(vShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    };

    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShaderProg, NULL);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fShader, 512, NULL, infoLog);
        DM_CORE_WARN("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n {0}", infoLog)
    };

    ID = glCreateProgram();
    glAttachShader(ID, vShader);
    glAttachShader(ID, fShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        DM_CORE_ERROR("ERROR LINKING SHADER: {0}", infoLog);
    }

    // load();

    glDeleteShader(vShader);
    glDeleteShader(fShader);
}

Shader::Shader(const std::string& vertexPath, const std::string& fragPath)
    : Asset(vertexPath + fragPath)
{
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

    unsigned int vShader, fShader;
    int result;
    char infoLog[512];

    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShaderProg, NULL);
    glCompileShader(vShader);

    glGetShaderiv(vShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    };

    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShaderProg, NULL);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fShader, 512, NULL, infoLog);
        DM_CORE_WARN("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n {0}", infoLog)
    };

    ID = glCreateProgram();
    glAttachShader(ID, vShader);
    glAttachShader(ID, fShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        DM_CORE_ERROR("ERROR LINKING SHADER: {0}", infoLog);
    }

    // load();

    glDeleteShader(vShader);
    glDeleteShader(fShader);
}

void Shader::load()
{

    // glm::mat4 projection = Camera::getProjection();
    // setMat4("projection", projection);
}

void Shader::use()
{
    glUseProgram(ID);
    load();
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

}
