#ifndef DM_SHADER_HPP
#define DM_SHADER_HPP

#include "nvrhi/nvrhi.h"
#include <slang/slang-com-ptr.h>
#include <slang/slang.h>

namespace Dimensional {

struct ShaderVarient;

struct ShaderData {
    std::string name;
    std::map<nvrhi::ShaderType, std::string> programs;
};

struct ShaderCreateInfo {
    std::vector<const char*> includePaths;
    std::vector<slang::PreprocessorMacroDesc> defines;
    int optimizationLevel = 3;
};

class Shader {
public:
    static Ref<Shader> Create(const std::string& path, const ShaderCreateInfo info = {});

    nvrhi::ShaderHandle GetShaderHandle(nvrhi::ShaderType type);
    ShaderVarient GetShaderVariant(nvrhi::ShaderType type);

private:
    bool Compile(std::string path, const ShaderCreateInfo info);

    Shader() = default;

    std::map<nvrhi::ShaderType, ShaderVarient> m_Shaders;

    std::string m_Name = "Default Shader Name";

    friend class GraphicsPipeline;
};

}

#endif // DM_SHADER_HPP
