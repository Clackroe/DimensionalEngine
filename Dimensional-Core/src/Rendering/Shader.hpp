#ifndef DM_SHADER_HPP
#define DM_SHADER_HPP

#include "nvrhi/nvrhi.h"
#include <shaderc/shaderc.hpp>

#include <spirv_cross.hpp>
#include <spirv_cross/spirv.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Dimensional {

enum ShaderType {
    NONE = 0,
    COMPUTE,
    VERTEX,
    FRAGMENT,
    GEOMETRY
};

struct ShaderData {
    std::string name;
    UMap<ShaderType, std::string> programs;
};

struct Shader {

    static Ref<Shader> Create(const std::string& path);

    nvrhi::ShaderHandle GetShaderHandle(ShaderType type);
    nvrhi::InputLayoutHandle GetInputLayout() { return m_InputLayout; };
    spirv_cross::ShaderResources GetShaderResources(ShaderType type);

private:
    bool Compile(const ShaderData& sources);
    bool CreateNVRHIShaders();
    bool GenerateInputLayouts();

    Shader() = default;

    std::map<ShaderType, std::vector<u32>> m_Assemblies;
    std::map<ShaderType, nvrhi::ShaderHandle> m_Shaders;

    std::map<ShaderType, spirv_cross::ShaderResources> m_ShaderResources;

    nvrhi::InputLayoutHandle m_InputLayout;

    std::string m_Name = "Default Shader Name";
};

}
#endif // DM_SHADER_HPP
