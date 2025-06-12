#ifndef DM_SHADER_COMPILER
#define DM_SHADER_COMPILER
#include "Rendering/ShaderReflection.hpp"
#include <filesystem>
#include <memory>
#include <nvrhi/nvrhi.h>
#include <slang/slang-com-ptr.h>
#include <slang/slang.h>
#include <string>
#include <vector>

namespace Dimensional {

struct EntryPointDescription {
    std::string name;
    nvrhi::ShaderType type;
    SlangStage stage;
};

struct ShaderCompileOptions {
    std::vector<const char*> includePaths;
    std::vector<slang::PreprocessorMacroDesc> definesMacros;
    int optimizationLevel = 3;
};

struct ShaderVarient {
    nvrhi::ShaderHandle handle;
    std::string entryPoint;
    nvrhi::ShaderType type;

    nvrhi::InputLayoutHandle inputLayout; // Useful for Vertex Shaders Only

    ShaderReflectionData reflection;
};

class ShaderCompiler {
public:
    ShaderCompiler();
    ~ShaderCompiler();

    ShaderVarient compileShader(nvrhi::IDevice* device, const std::string& filePath, const ShaderCompileOptions& options, EntryPointDescription entryPointDesc);

    std::map<nvrhi::ShaderType, ShaderVarient> compileAllEntryPoints(nvrhi::IDevice* device, const std::string& filePath, const ShaderCompileOptions& options);

private:
    Slang::ComPtr<slang::ISession> createSession(const std::string& path, const ShaderCompileOptions& options);
    Slang::ComPtr<slang::IModule> loadModule(const std::string& moduleName, Slang::ComPtr<slang::ISession> session);

    Slang::ComPtr<slang::IBlob> compileShaderToBlob(const std::string& filePath, Slang::ComPtr<slang::IModule> module, Slang::ComPtr<slang::ISession> slangSession, const EntryPointDescription& entryPointDesc, ShaderReflectionData* reflection);

    nvrhi::ShaderHandle createShaderFromBlob(nvrhi::IDevice* device, Slang::ComPtr<slang::IBlob> blob, const EntryPointDescription entryPointDesc);

    nvrhi::InputLayoutHandle createInputLaytout(nvrhi::IDevice* device, const ShaderReflectionData& data, nvrhi::ShaderHandle vertexHandle);

    std::vector<EntryPointDescription> discoverEntryPoints(Slang::ComPtr<slang::IModule> module);

private:
    static Slang::ComPtr<slang::IGlobalSession> s_slangGlobalSession;
};

}
#endif // DM_SHADER_COMPILER
