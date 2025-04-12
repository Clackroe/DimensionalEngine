
#include "Core/Application.hpp"
#include "Log/log.hpp"
#include "Rendering/Enums.hpp"
#include "Rendering/GlslIncluder.hpp"
#include "nvrhi/nvrhi.h"
#include <Rendering/Shader.hpp>
#include <Rendering/ShaderHelpersAndEnums.hpp>
#include <functional>
#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>
#include <shaderc/status.h>
#include <spirv.hpp>
#include <spirv_cross.hpp>

namespace Dimensional {

static ShaderData parseProgramSources(const std::string& path)
{

    std::ifstream file;
    file.open(path);
    if (!file.is_open()) {
        DM_CORE_WARN("Failed to load shader: {}", path);
    }

    std::stringstream stream;
    stream << file.rdbuf();
    file.close();

    ShaderData data = {};
    ShaderType currentShader = NONE;

    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty()) {
            continue;
        }

        if (line.find(ShaderTypeToSpecifier(VERTEX)) != std::string::npos) {
            currentShader = VERTEX;
            data.programs[currentShader] = std::string();
            continue;
        }
        if (line.find(ShaderTypeToSpecifier(FRAGMENT)) != std::string::npos) {
            currentShader = FRAGMENT;
            data.programs[currentShader] = std::string();
            continue;
        }
        if (line.find(ShaderTypeToSpecifier(GEOMETRY)) != std::string::npos) {
            currentShader = GEOMETRY;
            data.programs[currentShader] = std::string();
            continue;
        }

        if (line.find(ShaderTypeToSpecifier(COMPUTE)) != std::string::npos) {
            currentShader = COMPUTE;
            data.programs[currentShader] = std::string();
            continue;
        }

        if (currentShader != NONE) {
            data.programs[currentShader] += (line + "\n");
            continue;
        }
        DM_CORE_WARN("WARNING: POSSIBLE MISFORMATTED SHADERFILE")
    }

    data.name = std::filesystem::path(path).stem().string();
    return data;
}

static void ReflectOnDescriptors(ShaderSetReflectionData& reflectionData, const std::vector<u32>& bin, nvrhi::ShaderType visibility)
{
    spirv_cross::CompilerGLSL compiler(bin);
    const auto& resources = compiler.get_shader_resources();

    auto handleResource = [&](const spirv_cross::Resource& res, nvrhi::ResourceType resType, const char* resTypeName) {
        uint32_t set = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
        uint32_t slot = compiler.get_decoration(res.id, spv::DecorationBinding);

        auto& bindingSet = reflectionData.bindingSetToLayoutItems[set];
        auto& vis = reflectionData.bindingSetToVis[set];

        vis = (nvrhi::ShaderType)((u32)(vis) | (u32)(visibility));
        // vis = nvrhi::ShaderType::All;

        DM_CORE_INFO("\t Reflecting: Name = '{}', Set = {}, Binding = {}, Type = {}", res.name, set, slot, resTypeName);

        bool existsAlready = false;
        for (auto& bs : bindingSet) {
            if (bs.slot == slot && bs.type == resType) { // Already exists
                existsAlready = true;
            }
        }
        if (existsAlready) {
            return;
        }

        nvrhi::BindingLayoutItem item;
        item.slot = slot;
        item.type = resType;
        bindingSet.push_back(item);
    };

    for (const auto& ubo : resources.uniform_buffers)
        handleResource(ubo, nvrhi::ResourceType::ConstantBuffer, "Uniform Buffer");

    for (const auto& ssbo : resources.storage_buffers) {
        bool isWritten = compiler.has_decoration(ssbo.type_id, spv::DecorationNonReadable);
        nvrhi::ResourceType t = isWritten ? nvrhi::ResourceType::RawBuffer_UAV : nvrhi::ResourceType::RawBuffer_SRV;
        handleResource(ssbo, t, "SSBO");
    }

    for (const auto& image : resources.sampled_images) {
        handleResource(image, nvrhi::ResourceType::Texture_SRV, "Combined Sampler Images (SRV)");
    }

    for (const auto& uav : resources.storage_images)
        handleResource(uav, nvrhi::ResourceType::Texture_UAV, "Storage Images (UAV)");

    for (const auto& sepimg : resources.separate_images)
        handleResource(sepimg, nvrhi::ResourceType::Texture_SRV, "Separate Images (UAV)");

    for (const auto& sampler : resources.separate_samplers)
        handleResource(sampler, nvrhi::ResourceType::Sampler, "Seperate Samplers");
}

Ref<Shader> Shader::Create(const std::string& path)
{

    ShaderData sources = parseProgramSources(path);

    Ref<Shader> shader = Ref<Shader>(new Shader());
    shader->m_Name = sources.name;

    if (!shader->Compile(sources)) {
        return nullptr;
    }
    if (!shader->CreateNVRHIShaders()) {
        return nullptr;
    }
    if (!shader->GenerateInputLayouts()) {
        return nullptr;
    }
    DM_CORE_INFO("Shader Resource Data: ({})", sources.name);
    if (!shader->GenerateBindingLayouts()) {
        return nullptr;
    }

    return shader;
}

static GLSLIncluder includer("./Assets/Shaders");

bool Shader::Compile(const ShaderData& sources)
{
    shaderc::Compiler comp;
    shaderc::CompileOptions opt;
    opt.SetIncluder(CreateScope<GLSLIncluder>(includer));

    for (auto [type, source] : sources.programs) {

        shaderc::CompilationResult res = comp.CompileGlslToSpv(source, ShaderTypeToShaderC(type), sources.name.c_str(), opt);

        if (res.GetCompilationStatus() != shaderc_compilation_status_success) {
            DM_CORE_ERROR("Failed to compile Shader: {0}:{1} - {2}", sources.name, ShaderTypeToSpecifier(type), res.GetErrorMessage());
            return false;
        }
        m_Assemblies[type] = { res.cbegin(), res.cend() };
        spirv_cross::CompilerGLSL compiler({ res.cbegin(), res.cend() });
        m_ShaderResources[type] = compiler.get_shader_resources();
    }
    return true;
}
bool Shader::CreateNVRHIShaders()
{
    for (auto [type, assembly] : m_Assemblies) {

        spirv_cross::CompilerGLSL glsl(assembly);

        nvrhi::ShaderDesc desc;
        desc.shaderType = ShaderTypeToNVRHI(type);
        desc.debugName = m_Name + "_" + ShaderTypeToString(type);
        desc.entryName = "main";
        std::vector<u32> data = assembly;

        auto shade = Application::getDevice()->createShader(desc, data.data(), data.size() * sizeof(u32));

        if (!shade) {
            DM_CORE_ERROR("Failed to create shader: {}", desc.debugName);
            return false;
        }
        m_Shaders[type] = shade;
    }

    return true;
}

bool Shader::GenerateBindingLayouts()
{

    ShaderSetReflectionData data;
    for (auto& [type, bin] : m_Assemblies) {
        ReflectOnDescriptors(data, bin, ShaderTypeToNVRHI(type));
    }
    std::vector<std::vector<nvrhi::BindingLayoutItem>> sortedItems;
    u32 maxSet = 0;
    for (auto& [set, items] : data.bindingSetToLayoutItems) {
        maxSet = std::max(maxSet, set);
    }
    sortedItems.resize(maxSet + 1);

    // Should be sorted, using std::map
    for (auto& [set, items] : data.bindingSetToLayoutItems) {
        std::stable_sort(items.begin(), items.end(), [](const auto& a, const auto& b) {
            return a.slot < b.slot;
        });
        sortedItems[set] = (items);
    }

    for (u32 i = 0; i < sortedItems.size(); i++) {
        auto& items = sortedItems[i];
        nvrhi::BindingLayoutDesc desc;

        if (items.size() <= 0) {
            desc.setVisibility(nvrhi::ShaderType::All);
            auto b = Application::getDevice()->createBindingLayout(desc);
            if (!b) {
                DM_CORE_WARN("Failed to create filler Binding Layout for shader: {}", m_Name);
                return false;
            }
            m_BindingLayouts.push_back(b);
            continue;
        }
        if (data.bindingSetToVis.contains(i)) {
            desc.setVisibility(data.bindingSetToVis.at(i));
        } else {
            desc.setVisibility(nvrhi::ShaderType::All);
        }

        for (auto& item : items) {
            desc.addItem(item);
        }

        auto b = Application::getDevice()->createBindingLayout(desc);
        if (!b) {
            DM_CORE_WARN("Failed to create Binding Layout for shader: {}", m_Name);
            continue;
        }
        m_BindingLayouts.push_back(b);
    }

    return true;
}

bool Shader::GenerateInputLayouts()
{
    // Currently only supporting Vertex Shaders (Not sure what the other use cases would be?)
    if (!m_ShaderResources.contains(ShaderType::VERTEX)) {
        return true;
    }

    spirv_cross::ShaderResources res = GetShaderResources(ShaderType::VERTEX);
    spirv_cross::CompilerGLSL compiler({ m_Assemblies.at(ShaderType::VERTEX) });

    std::vector<std::pair<nvrhi::VertexAttributeDesc, u32>> att_data;

    for (const auto& input : res.stage_inputs) {
        u32 loc = compiler.get_decoration(input.id, spv::DecorationLocation);
        const auto& type = compiler.get_type(input.type_id);
        nvrhi::Format format = getSpirvToNvrhiFormat(type);
        std::string name = compiler.get_name(input.id);

        if (name.empty()) {
            name = input.name;
        }

        nvrhi::VertexAttributeDesc desc;
        desc.name = name.c_str();
        desc.format = format;
        desc.arraySize = 1;
        desc.offset = spirvTypeToSize(type); // Will consolodate in the next loop.
        att_data.push_back({ desc, loc });
    }

    std::sort(att_data.begin(), att_data.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    u32 totalStride = 0;
    u32 offset = 0;
    for (auto& [desc, location] : att_data) {
        u32 temp = desc.offset;
        desc.offset = offset;
        offset += temp;
        totalStride += temp;
    }

    for (auto& [desc, location] : att_data) {
        desc.elementStride = totalStride;
    }

    DM_CORE_INFO("SHADER INPUTS: ({})", m_Name);
    for (auto& [desc, location] : att_data) {
        DM_CORE_INFO("\tName: {}", desc.name);
        DM_CORE_INFO("\t\tLocation: {}", location);
        DM_CORE_INFO("\t\tFormat: {}", NVRHIFormatToString(desc.format));
        DM_CORE_INFO("\t\tOffset: {}", desc.offset);
        DM_CORE_INFO("\t\tStride: {}", desc.elementStride);
    }

    std::vector<nvrhi::VertexAttributeDesc> atts;
    for (auto& [desc, location] : att_data) {
        atts.push_back(desc);
    }

    m_InputLayout = Application::getDevice()->createInputLayout(atts.data(), atts.size(), GetShaderHandle(ShaderType::VERTEX));
    if (!m_InputLayout) {
        DM_CORE_WARN("Failed to create Input Layout on Shader: {}", m_Name);
        return false;
    }

    return true;
}

nvrhi::ShaderHandle Shader::GetShaderHandle(ShaderType type)
{
    if (m_Shaders.contains(type)) {
        return m_Shaders.at(type);
    }

    DM_CORE_WARN("Tried to access nvrhi::shader on shader that doesnt contain it: {0}: {1}", m_Name, ShaderTypeToSpecifier(type))
    return nullptr;
}

spirv_cross::ShaderResources Shader::GetShaderResources(ShaderType type)
{
    if (m_ShaderResources.contains(type)) {
        return m_ShaderResources.at(type);
    }

    DM_CORE_WARN("Tried to access ShaderResource on shader that doesnt contain it: {0}: {1}", m_Name, ShaderTypeToSpecifier(type))
    return spirv_cross::ShaderResources();
}

nvrhi::ShaderHandle Shader::GetShader(ShaderType type)
{
    if (m_ShaderResources.contains(type)) {
        return m_Shaders.at(type);
    }

    DM_CORE_WARN("Tried to access Shader on shader that doesnt contain it: {0}: {1}", m_Name, ShaderTypeToSpecifier(type))
    return nullptr;
}

}
