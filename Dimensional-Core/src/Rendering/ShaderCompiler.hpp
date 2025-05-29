#ifndef DM_SHADER_COMPILER
#define DM_SHADER_COMPILER
#include <filesystem>
#include <memory>
#include <nvrhi/nvrhi.h>
#include <slang-com-ptr.h>
#include <slang.h>
#include <string>
#include <vector>

namespace Dimensional {

struct EntryPointDescription {
    std::string entryPoint;
    nvrhi::ShaderType type;
};

struct ShaderCompileOptions {
    EntryPointDescription entryPoint;
    std::vector<const char*> includePaths;
    std::vector<slang::PreprocessorMacroDesc> definesMacros;
    int optimizationLevel = 3;
    bool enableDebugInfo = false;
};

struct ShaderVarient {
    nvrhi::ShaderHandle handle;
    std::string entryPoint;
    nvrhi::ShaderType type;
};

struct ShaderBlob {
    std::vector<u32> data;
    std::string entryPoint;
    nvrhi::ShaderType type;
};

class ShaderCompiler {
public:
    ShaderCompiler();
    ~ShaderCompiler();

    ShaderVarient compileShader(nvrhi::IDevice* device, const std::string& filePath, const ShaderCompileOptions& options);

private:
    Slang::ComPtr<slang::IBlob> compileShaderToBlob(const std::string& filePath, const ShaderCompileOptions& options);
    nvrhi::ShaderHandle createShaderFromBlob(nvrhi::IDevice* device, Slang::ComPtr<slang::IBlob> blob, const EntryPointDescription entryPointDesc);

private:
    static Slang::ComPtr<slang::IGlobalSession> s_slangGlobalSession;
};

}
#endif // DM_SHADER_COMPILER
