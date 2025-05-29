#ifndef DM_SHADER_HPP
#define DM_SHADER_HPP

#include "nvrhi/nvrhi.h"
#include <slang-com-ptr.h>
#include <slang.h>

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

// struct ShaderSetReflectionData {
//     UMap<u32, nvrhi::BindingLayoutDesc> bindingSetDescs;
// };

struct ShaderCreateInfo {
    const std::vector<const char*>& includePaths = {};
    const std::vector<slang::PreprocessorMacroDesc>& defines = {};
    int optimizationLevel = 3;
    bool debugInfo = true;
};

class Shader {
public:
    static Ref<Shader> Create(const std::string& path, const ShaderCreateInfo info = {});

    nvrhi::ShaderHandle GetShaderHandle(ShaderType type);

private:
    bool Compile(std::string path, const ShaderCreateInfo info);
    // bool CompileWithSlang(const ShaderData& sources);
    bool CreateNVRHIShaders();

    Shader() = default;

    std::map<ShaderType, nvrhi::ShaderHandle> m_Shaders;

    std::string m_Name = "Default Shader Name";
};

}

#endif // DM_SHADER_HPP
