#ifndef DM_SHADER_HPP
#define DM_SHADER_HPP

#include "nvrhi/nvrhi.h"
#include <slang/slang-com-ptr.h>
#include <slang/slang.h>

namespace Dimensional {

struct ShaderResource;

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
    const ShaderVarient& GetShaderVariant(nvrhi::ShaderType type);

private:
    bool Compile(std::string path, const ShaderCreateInfo info);

    void retrieveResources();
    bool createLayouts();

    Shader() = default;

    std::map<nvrhi::ShaderType, ShaderVarient> m_Shaders;

    std::map<u32, nvrhi::BindingLayoutHandle> m_Layouts; // space/set -> BindingLayout

    std::map<u32, std::map<u32, ShaderResource>> m_Resources; // space/set => (Binding slot -> Resource)

    std::string m_Name = "Default Shader Name";

    friend class GraphicsPipeline;
};

}

#endif // DM_SHADER_HPP
