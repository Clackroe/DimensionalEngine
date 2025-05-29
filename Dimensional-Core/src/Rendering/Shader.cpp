#include "Rendering/Shader.hpp"
#include "Core/Application.hpp"
#include "Rendering/ShaderCompiler.hpp"

namespace Dimensional {

Ref<Shader> Shader::Create(const std::string& path, const ShaderCreateInfo info)
{

    std::filesystem::path fs_path(path);

    Ref<Shader> shader = Ref<Shader>(new Shader());
    shader->m_Name = fs_path.filename();

    if (!shader->Compile(path, info)) {
        return nullptr;
    }

    // if (!shader->CreateNVRHIShaders()) {
    //     return nullptr;
    // }
    //
    // if (!shader->GenerateInputLayouts()) {
    //     return nullptr;
    // }
    //
    // DM_CORE_INFO("Shader Resource Data: ({})", shader->m_Name);
    //
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
    options.enableDebugInfo = info.debugInfo;
    options.includePaths = info.includePaths;
    options.optimizationLevel = info.optimizationLevel;

    // options.shaderType;
    // options.entryPoint;

    sc.compileShader(Application::getDeviceManager()->GetDevice(), path, options);

    return true;
}

// bool Shader::CompileWithSlang(const ShaderData& sources)
// {
//     ShaderCompiler sc;
//
//     auto dev = Application::getDeviceManager();
//     sc.compileShader(dev->GetDevice(), const ShaderCompileOptions& options)
// }

}
