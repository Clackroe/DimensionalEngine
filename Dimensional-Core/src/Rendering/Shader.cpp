
#include "Rendering/Shader.hpp"
#include "Log/log.hpp"
#include <Rendering/ShaderManager.hpp>
#include <string>

namespace Dimensional {

static UMap<ShaderType, std::string> ShaderTypeToSpecifier = {
    { COMPUTE, "##COMPUTE" },
    { VERTEX, "##VERTEXSHADER" },
    { FRAGMENT, "##FRAGSHADER" },
    { GEOMETRY, "##GEOMETRY" },
};

static ShaderData parseProgramSources(const std::string& path)
{

    std::ifstream file;
    file.open(path);
    if (!file.is_open()) {
        DM_CORE_WARN("Failed to load shader: {}", path);
    }

    std::stringstream stream;
    stream << file.rdbuf();
    file.close();

    ShaderData data = {};
    ShaderType currentShader = NONE;

    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty()) {
            continue;
        }

        if (line.find(ShaderTypeToSpecifier.at(VERTEX)) != std::string::npos) {
            currentShader = VERTEX;
            data.programs[currentShader] = std::string();
            continue;
        }
        if (line.find(ShaderTypeToSpecifier.at(FRAGMENT)) != std::string::npos) {
            currentShader = FRAGMENT;
            data.programs[currentShader] = std::string();
            continue;
        }
        if (line.find(ShaderTypeToSpecifier.at(GEOMETRY)) != std::string::npos) {
            currentShader = GEOMETRY;
            data.programs[currentShader] = std::string();
            continue;
        }

        if (line.find(ShaderTypeToSpecifier.at(COMPUTE)) != std::string::npos) {
            currentShader = COMPUTE;
            data.programs[currentShader] = std::string();
            continue;
        }

        if (currentShader != NONE) {
            data.programs[currentShader] += (line + "\n");
            continue;
        }
        DM_CORE_WARN("WARNING: POSSIBLE MISFORMATTED SHADERFILE")
    }

    data.name = std::filesystem::path(path).stem().string();
    return data;
}

Ref<Shader> Shader::Create(const std::string& path)
{
    Ref<Shader> shader = Ref<Shader>(new Shader());
    shader->m_ID = ShaderManager::CreateShader(parseProgramSources(path));
    return shader;
}

void Shader::Bind()
{
    ShaderManager::Bind(m_ID);
}
void Shader::UnBind()
{
    ShaderManager::UnBind(m_ID);
}

}
