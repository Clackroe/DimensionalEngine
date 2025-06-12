#include "Rendering/Shader.hpp"
#include "Core/Application.hpp"
#include "Rendering/ShaderCompiler.hpp"
#include "Rendering/ShaderHelpersAndEnums.hpp"

namespace Dimensional {

Ref<Shader> Shader::Create(const std::string& path, const ShaderCreateInfo info)
{

    std::filesystem::path fs_path(path);

    Ref<Shader> shader = Ref<Shader>(new Shader());
    shader->m_Name = fs_path.filename();

    if (!shader->Compile(path, info)) {
        return nullptr;
    }

    // if (!shader->GenerateBindingLayouts()) {
    //     return nullptr;
    // }

    return shader;
}

bool Shader::Compile(std::string path, const ShaderCreateInfo info)
{
    ShaderCompiler sc;
    ShaderCompileOptions options;
    options.definesMacros = info.defines;
    options.includePaths = info.includePaths;
    options.optimizationLevel = info.optimizationLevel;

    m_Shaders = sc.compileAllEntryPoints(Application::getDeviceManager()->GetDevice(), path, options);

    return true;
}

nvrhi::ShaderHandle Shader::GetShaderHandle(nvrhi::ShaderType type)
{
    if (!m_Shaders.contains(type)) {
        DM_CORE_WARN("Tried to get shader handle of type {} when it doesnt exists", ShaderTypeToString(type))
    }
    return m_Shaders.at(type).handle;
}
ShaderVarient Shader::GetShaderVariant(nvrhi::ShaderType type)
{
    if (!m_Shaders.contains(type)) {
        DM_CORE_WARN("Tried to get shader variant of type {} when it doesnt exists", ShaderTypeToString(type))
    }
    return m_Shaders.at(type);
}

}
