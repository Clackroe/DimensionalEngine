#include "ShaderCompiler.hpp"
#include "Log/log.hpp"
#include "Rendering/ShaderReflection.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ios>
#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>
#include <slang/slang.h>
#include <string>
#include <vector>

namespace Dimensional {

static nvrhi::ShaderType slangStageToNvrhiType(SlangStage stage)
{
    switch (stage) {
    case SLANG_STAGE_VERTEX:
        return nvrhi::ShaderType::Vertex;
    case SLANG_STAGE_FRAGMENT:
        return nvrhi::ShaderType::Pixel;
    case SLANG_STAGE_COMPUTE:
        return nvrhi::ShaderType::Compute;
    case SLANG_STAGE_GEOMETRY:
        return nvrhi::ShaderType::Geometry;
    case SLANG_STAGE_HULL:
        return nvrhi::ShaderType::Hull;
    case SLANG_STAGE_DOMAIN:
        return nvrhi::ShaderType::Domain;
    default:
        return nvrhi::ShaderType::None;
    }
}

Slang::ComPtr<slang::IGlobalSession> ShaderCompiler::s_slangGlobalSession;

ShaderCompiler::ShaderCompiler()
{
    if (!s_slangGlobalSession) {
        slang::createGlobalSession(s_slangGlobalSession.writeRef());
        if (!s_slangGlobalSession) {
            DM_CORE_ERROR("Failed to create slang global session");
        }
    }
}

ShaderCompiler::~ShaderCompiler()
{
}

ShaderVarient ShaderCompiler::compileShader(nvrhi::IDevice* device, const std::string& filePath, const ShaderCompileOptions& options, EntryPointDescription entryPointDesc)
{
    ShaderReflectionData reflection;

    Slang::ComPtr<slang::ISession> slangSession = createSession(filePath, options);
    Slang::ComPtr<slang::IModule> module = loadModule(filePath, slangSession);

    auto shaderBlob = compileShaderToBlob(filePath, module, slangSession, entryPointDesc, &reflection);

    ShaderReflector::printReflection(reflection);

    if (shaderBlob->getBufferSize() <= 0) {
        DM_CORE_ERROR("Failed to compile shader to blob: {0}", filePath);
        return ShaderVarient {};
    }

    nvrhi::ShaderHandle handle = createShaderFromBlob(device, shaderBlob, entryPointDesc);

    nvrhi::InputLayoutHandle inputLayout;
    if (entryPointDesc.type == nvrhi::ShaderType::Vertex) {
        inputLayout = createInputLaytout(device, reflection, handle);
    }

    ShaderVarient varient;
    varient.entryPoint = entryPointDesc.name;
    varient.handle = handle;
    varient.type = entryPointDesc.type;
    varient.reflection = reflection;
    varient.inputLayout = inputLayout;

    return varient;
}

std::map<nvrhi::ShaderType, ShaderVarient> ShaderCompiler::compileAllEntryPoints(nvrhi::IDevice* device, const std::string& filePath, const ShaderCompileOptions& options)
{
    ShaderReflectionData reflection;

    Slang::ComPtr<slang::ISession> slangSession = createSession(filePath, options);
    Slang::ComPtr<slang::IModule> module = loadModule(filePath, slangSession);

    std::vector<EntryPointDescription> entryPoints = discoverEntryPoints(module);

    std::map<nvrhi::ShaderType, ShaderVarient> variants;

    for (auto ep : entryPoints) {

        auto shaderBlob = compileShaderToBlob(filePath, module, slangSession, ep, &reflection);
        ShaderReflector::printReflection(reflection);

        if (shaderBlob->getBufferSize() <= 0) {
            DM_CORE_ERROR("Failed to compile shader to blob: {0}", filePath);
            return {};
        }

        nvrhi::ShaderHandle handle = createShaderFromBlob(device, shaderBlob, ep);

        nvrhi::InputLayoutHandle inputLayout;
        if (ep.type == nvrhi::ShaderType::Vertex) {
            inputLayout = createInputLaytout(device, reflection, handle);
        }

        ShaderVarient varient;
        varient.entryPoint = ep.name;
        varient.handle = handle;
        varient.type = ep.type;
        varient.reflection = reflection;
        varient.inputLayout = inputLayout;

        variants.insert({ ep.type, std::move(varient) });
    }
    return variants;
}

static int _packInt3(uint8_t upper, int lower)
{
    return ((u32)(upper) << 24) | (lower & 0xFFFFFF);
}

enum class _ShiftKind {
    Invalid = -1,

    /// Unordered access view (u)
    ///
    /// RWByteAddressBuffer/RWStructuredBuffer
    /// Append/ConsumeStructuredBuffer
    /// RWBuffer
    /// RWTextureXD/Array
    UnorderedAccess = 0,

    /// Sampler (s)
    ///
    /// SamplerXD
    /// SamplerState/SamplerComparisonState
    Sampler,

    /// Shader Resource (t)
    ///
    /// TextureXD/Array
    /// ByteAddressBuffer/StructuredBuffer/Buffer/TBuffer
    ShaderResource,

    /// Constant buffer (b)
    ///
    /// ConstantBufferViews, CBuffer
    ConstantBuffer,

    CountOf,
};

#define VKSHIFTOPTION(space, shift, kindR) {                                                                                                                                          \
    .name = slang::CompilerOptionName::VulkanBindShift, .value = { .kind = slang::CompilerOptionValueKind::Int, .intValue0 = _packInt3((u8)(kindR), space), .intValue1 = i32(shift) } \
},

Slang::ComPtr<slang::ISession> ShaderCompiler::createSession(const std::string& filePath, const ShaderCompileOptions& options)
{
    if (!std::filesystem::exists(filePath)) {
        DM_CORE_ERROR("Shader file does not exist: {0}", filePath);
        return {};
    }

    slang::SessionDesc sessionDesc = {};

    slang::TargetDesc targetDesc = {};
    // TODO: Support other Graphics APIs
    targetDesc.format = SLANG_SPIRV;
    targetDesc.profile = s_slangGlobalSession->findProfile("spirv_1_4");

    slang::CompilerOptionEntry entries[] = {
        {
            .name = slang::CompilerOptionName::VulkanUseEntryPointName,
            .value = {
                .kind = slang::CompilerOptionValueKind::Int,
                .intValue0 = 1 //
            } //
        },
        {
            .name = slang::CompilerOptionName::Optimization, //
            .value = {
                .kind = slang::CompilerOptionValueKind::Int,
                .intValue0 = options.optimizationLevel //
            } //
        },
        VKSHIFTOPTION(0, SHADER_RESOURCE_OFFSET, _ShiftKind::ShaderResource)
        //
        VKSHIFTOPTION(1, SHADER_RESOURCE_OFFSET, _ShiftKind::ShaderResource)
        //
        VKSHIFTOPTION(2, SHADER_RESOURCE_OFFSET, _ShiftKind::ShaderResource)
        //
        VKSHIFTOPTION(3, SHADER_RESOURCE_OFFSET, _ShiftKind::ShaderResource)
        //
        VKSHIFTOPTION(4, SHADER_RESOURCE_OFFSET, _ShiftKind::ShaderResource)

        //
        VKSHIFTOPTION(0, SAMPLER_OFFSET, _ShiftKind::Sampler)
        //
        VKSHIFTOPTION(1, SAMPLER_OFFSET, _ShiftKind::Sampler)
        //
        VKSHIFTOPTION(2, SAMPLER_OFFSET, _ShiftKind::Sampler)
        //
        VKSHIFTOPTION(3, SAMPLER_OFFSET, _ShiftKind::Sampler)
        //
        VKSHIFTOPTION(4, SAMPLER_OFFSET, _ShiftKind::Sampler)

        //
        VKSHIFTOPTION(0, CONSTANTBUFFER_OFFSET, _ShiftKind::ConstantBuffer)
        //
        VKSHIFTOPTION(1, CONSTANTBUFFER_OFFSET, _ShiftKind::ConstantBuffer)
        //
        VKSHIFTOPTION(2, CONSTANTBUFFER_OFFSET, _ShiftKind::ConstantBuffer)
        //
        VKSHIFTOPTION(3, CONSTANTBUFFER_OFFSET, _ShiftKind::ConstantBuffer)
        //
        VKSHIFTOPTION(4, CONSTANTBUFFER_OFFSET, _ShiftKind::ConstantBuffer)

        //
        VKSHIFTOPTION(0, UNORDEREDACCESS_OFFSET, _ShiftKind::UnorderedAccess)
        //
        VKSHIFTOPTION(1, UNORDEREDACCESS_OFFSET, _ShiftKind::UnorderedAccess)
        //
        VKSHIFTOPTION(2, UNORDEREDACCESS_OFFSET, _ShiftKind::UnorderedAccess)
        //
        VKSHIFTOPTION(3, UNORDEREDACCESS_OFFSET, _ShiftKind::UnorderedAccess)
        //
        VKSHIFTOPTION(4, UNORDEREDACCESS_OFFSET, _ShiftKind::UnorderedAccess)

    };
    targetDesc.compilerOptionEntries = entries;
    targetDesc.compilerOptionEntryCount = std::size(entries); // entries.size();

    sessionDesc.targets = &targetDesc;
    sessionDesc.targetCount = 1;

    // Set up search paths - need to convert to proper format
    std::vector<const char*> searchPaths;
    for (const auto& path : options.includePaths) {
        searchPaths.push_back(path);
    }

    sessionDesc.searchPathCount = static_cast<SlangInt>(searchPaths.size());
    sessionDesc.searchPaths = searchPaths.data();

    sessionDesc.preprocessorMacroCount = static_cast<SlangInt>(options.definesMacros.size());
    sessionDesc.preprocessorMacros = options.definesMacros.data();

    Slang::ComPtr<slang::ISession> slangSession;
    SlangResult sessionResult = s_slangGlobalSession->createSession(sessionDesc, slangSession.writeRef());
    if (SLANG_FAILED(sessionResult)) {
        DM_CORE_ERROR("Failed to create Slang session for shader: {0}", filePath);
        return {};
    }

    return slangSession;
}

Slang::ComPtr<slang::IModule> ShaderCompiler::loadModule(const std::string& moduleName, Slang::ComPtr<slang::ISession> session)
{

    Slang::ComPtr<slang::IBlob> diagnostics;

    Slang::ComPtr<slang::IModule> module;
    module = session->loadModule(moduleName.c_str(), diagnostics.writeRef());

    if (diagnostics && diagnostics->getBufferSize() > 1) {
        DM_CORE_WARN("Slang module diagnostics:\n{0}", static_cast<const char*>(diagnostics->getBufferPointer()));
    }

    if (!module) {
        DM_CORE_ERROR("Failed to load module from shader module: {0}", moduleName);
        return {};
    }

    return module;
}

Slang::ComPtr<slang::IBlob> ShaderCompiler::compileShaderToBlob(const std::string& filePath, Slang::ComPtr<slang::IModule> module, Slang::ComPtr<slang::ISession> slangSession, const EntryPointDescription& entryPointDesc, ShaderReflectionData* reflection)
{

    Slang::ComPtr<slang::IBlob> diagnostics;

    std::string moduleName = std::filesystem::path(filePath).stem().string();

    Slang::ComPtr<slang::IEntryPoint> entryPoint;
    module->findEntryPointByName(entryPointDesc.name.c_str(), entryPoint.writeRef());

    if (!entryPoint) {
        DM_CORE_ERROR("Entry point '{0}' not found in shader: {1}", entryPointDesc.name, filePath);
        return {};
    }

    slang::IComponentType* componentTypes[] = { module, entryPoint };
    Slang::ComPtr<slang::IComponentType> linked;
    SlangResult linkResult = slangSession->createCompositeComponentType(componentTypes, 2, linked.writeRef(), diagnostics.writeRef());

    // Reflection
    *reflection = ShaderReflector::extractFromProgram(linked, entryPointDesc);

    if (diagnostics && diagnostics->getBufferSize() > 1) {
        DM_CORE_WARN("Linking diagnostics for {0}:\n{1}",
            entryPointDesc.name, static_cast<const char*>(diagnostics->getBufferPointer()));
    }

    if (SLANG_FAILED(linkResult)) {
        DM_CORE_ERROR("Failed to link shader components for: {0}", filePath);
        return {};
    }

    Slang::ComPtr<slang::IBlob> spirvBlob;
    Slang::ComPtr<slang::IBlob> compileDiagnostics;
    SlangResult result = linked->getTargetCode(0, /** entry point index*/ spirvBlob.writeRef(), compileDiagnostics.writeRef());

    if (compileDiagnostics && compileDiagnostics->getBufferSize() > 1) {
        DM_CORE_WARN("Compilation diagnostics for {0}:\n{1}",
            filePath, static_cast<const char*>(compileDiagnostics->getBufferPointer()));
    }

    if (SLANG_FAILED(result) || !spirvBlob) {
        DM_CORE_ERROR("Shader compilation failed for {0}", filePath);
        return {};
    }

    // TODO: Support DX
    //  Validate SPIR-V blob size and alignment
    const size_t blobSize = spirvBlob->getBufferSize();
    if (blobSize == 0) {
        DM_CORE_ERROR("Empty SPIR-V blob for shader: {0}", filePath);
        return {};
    }

    if (blobSize % 4 != 0) {
        DM_CORE_ERROR("Invalid SPIR-V blob size (not multiple of 4 bytes): {0}", blobSize);
        return {};
    }

    return spirvBlob;
}

nvrhi::ShaderHandle ShaderCompiler::createShaderFromBlob(nvrhi::IDevice* device, Slang::ComPtr<slang::IBlob> blob, const EntryPointDescription entryPointDesc)
{
    if (blob->getBufferSize() <= 0) {
        DM_CORE_ERROR("Cannot create shader from empty blob for entry point: {0}", entryPointDesc.name);
        return nullptr;
    }

    nvrhi::ShaderDesc desc;
    desc.entryName = entryPointDesc.name;
    desc.shaderType = entryPointDesc.type;
    desc.debugName = entryPointDesc.name;

    auto handle = device->createShader(desc, blob->getBufferPointer(), blob->getBufferSize());

    if (!handle) {
        DM_CORE_ERROR("Failed to create NVRHI shader for entry point {0} (blob size: {1} bytes)", entryPointDesc.name, blob->getBufferSize());
    } else {
        DM_CORE_INFO("Successfully created NVRHI shader for entry point: {0}", entryPointDesc.name);
    }

    return handle;
}

std::vector<EntryPointDescription> ShaderCompiler::discoverEntryPoints(Slang::ComPtr<slang::IModule> module)
{
    std::vector<EntryPointDescription> entryPoints;

    SlangInt entryPointCount = module->getDefinedEntryPointCount();
    for (SlangInt i = 0; i < entryPointCount; i++) {
        Slang::ComPtr<slang::IEntryPoint> entryPoint;
        module->getDefinedEntryPoint(i, entryPoint.writeRef());

        if (!entryPoint)
            continue;

        const char* name = entryPoint->getFunctionReflection()->getName();
        SlangStage stage = entryPoint->getLayout()->getEntryPointByIndex(0)->getStage();

        nvrhi::ShaderType nvrhiType = slangStageToNvrhiType(stage);

        if (nvrhiType != nvrhi::ShaderType::None) {
            EntryPointDescription info;
            info.name = name ? name : "";
            info.stage = stage;
            info.type = nvrhiType;
            entryPoints.push_back(info);
        } else {
            DM_CORE_WARN("Unsupported shader stage for entry point: {0}", name ? name : "unknown");
        }
    }

    for (auto t : entryPoints) {
        DM_CORE_WARN("\tName: {}", t.name);
        DM_CORE_WARN("\tNVType: {}", (int)t.type);
        DM_CORE_WARN("\tSlangStage: {}", (int)t.stage);
    }

    return entryPoints;
}

nvrhi::InputLayoutHandle ShaderCompiler::createInputLaytout(nvrhi::IDevice* device, const ShaderReflectionData& data, nvrhi::ShaderHandle vertexHandle)
{
    if (data.type != nvrhi::ShaderType::Vertex) {
        DM_CORE_INFO("Attempted to create an inputLayout on a non-vertex shader");
        return nullptr;
    }
    u64 totalSize = 0;
    for (auto input : data.vertexInputs) {
        totalSize += input.size;
    }

    std::vector<nvrhi::VertexAttributeDesc> vertexLayout = {};

    int numInputs = data.vertexInputs.size();
    for (int i = 0; i < numInputs; i++) {
        auto input = data.vertexInputs[i];
        for (auto att : input.attributes) {
            auto attribute = nvrhi::VertexAttributeDesc()
                                 .setName(att.name)
                                 .setFormat(att.format)
                                 .setOffset(att.offset)
                                 .setBufferIndex(i)
                                 .setElementStride(input.size);
            vertexLayout.push_back(std::move(attribute));
        }
    };
    nvrhi::InputLayoutHandle inputLayout = device->createInputLayout(vertexLayout.data(), vertexLayout.size(), vertexHandle);
    return inputLayout;
}

}
