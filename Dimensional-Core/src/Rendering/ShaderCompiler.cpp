#include "ShaderCompiler.hpp"
#include "Rendering/ShaderReflection.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ios>
#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>
#include <slang.h>
#include <vector>

namespace Dimensional {

Slang::ComPtr<slang::IGlobalSession> ShaderCompiler::s_slangGlobalSession;

ShaderCompiler::ShaderCompiler()
{
    // Initialize Slang global session
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

ShaderVarient ShaderCompiler::compileShader(nvrhi::IDevice* device, const std::string& filePath, const ShaderCompileOptions& options)
{
    auto shaderBlob = compileShaderToBlob(filePath, options);

    if (shaderBlob->getBufferSize() <= 0) {
        DM_CORE_ERROR("Failed to compile shader to blob: {0}", filePath);
        return ShaderVarient {};
    }

    nvrhi::ShaderHandle handle = createShaderFromBlob(device, shaderBlob, options.entryPointDesc);

    ShaderVarient varient;
    varient.entryPoint = options.entryPointDesc.name;
    varient.handle = handle;
    varient.type = options.entryPointDesc.type;

    return varient;
}

Slang::ComPtr<slang::IBlob> ShaderCompiler::compileShaderToBlob(const std::string& filePath, const ShaderCompileOptions& options)
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

    slang::CompilerOptionValue val;
    val.kind = slang::CompilerOptionValueKind::Int;
    val.intValue0 = 1;
    slang::CompilerOptionEntry ent = {
        .name = slang::CompilerOptionName::VulkanUseEntryPointName,
        .value = val
    };
    targetDesc.compilerOptionEntries = &ent;

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

    Slang::ComPtr<slang::IBlob> diagnostics;
    std::string moduleName = std::filesystem::path(filePath).stem().string();

    slang::IModule* module = slangSession->loadModule(
        moduleName.c_str(),
        diagnostics.writeRef());

    if (diagnostics && diagnostics->getBufferSize() > 1) {
        DM_CORE_WARN("Slang module diagnostics:\n{0}", static_cast<const char*>(diagnostics->getBufferPointer()));
    }

    if (!module) {
        DM_CORE_ERROR("Failed to load module from shader file: {0}", filePath);
        return {};
    }

    Slang::ComPtr<slang::IEntryPoint> entryPoint;
    module->findEntryPointByName(options.entryPointDesc.name.c_str(), entryPoint.writeRef());

    if (!entryPoint) {
        DM_CORE_ERROR("Entry point '{0}' not found in shader: {1}", options.entryPointDesc.name, filePath);
        return {};
    }

    slang::IComponentType* componentTypes[] = { module, entryPoint };
    Slang::ComPtr<slang::IComponentType> linked;
    SlangResult linkResult = slangSession->createCompositeComponentType(componentTypes, 2, linked.writeRef(), diagnostics.writeRef());

    if (diagnostics && diagnostics->getBufferSize() > 1) {
        DM_CORE_WARN("Linking diagnostics for {0}:\n{1}",
            options.entryPointDesc.name, static_cast<const char*>(diagnostics->getBufferPointer()));
    }

    if (SLANG_FAILED(linkResult)) {
        DM_CORE_ERROR("Failed to link shader components for: {0}", filePath);
        return {};
    }

    // ==== TEMPORARY ====
    auto t = ShaderReflection::extractFromProgram(linked, options.entryPointDesc);
    // ===================

    t.printReflectionInfo();
    auto s = t.getReflectionSummary();
    DM_CORE_INFO("{}", s)

    // Get target code with proper error handling
    Slang::ComPtr<slang::IBlob>
        spirvBlob;
    Slang::ComPtr<slang::IBlob> compileDiagnostics;
    SlangResult result = linked->getTargetCode(
        0, // entry point index
        spirvBlob.writeRef(),
        compileDiagnostics.writeRef());

    if (compileDiagnostics && compileDiagnostics->getBufferSize() > 1) {
        DM_CORE_WARN("Compilation diagnostics for {0}:\n{1}",
            filePath, static_cast<const char*>(compileDiagnostics->getBufferPointer()));
    }

    if (SLANG_FAILED(result) || !spirvBlob) {
        DM_CORE_ERROR("Shader compilation failed for {0}", filePath);
        return {};
    }

    // Validate SPIR-V blob size and alignment
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
    // TODO: Potential issues here, when compiling to SPIR-V for Vulkan, it changes
    // the entry point to "main" even with the necessary compile option. Maybe bug?
    desc.entryName = "main";
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
}
