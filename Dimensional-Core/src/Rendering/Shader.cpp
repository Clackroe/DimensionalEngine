#include "Rendering/Shader.hpp"
#include "Core/Application.hpp"
#include "Log/log.hpp"
#include "Rendering/ShaderCompiler.hpp"
#include "Rendering/ShaderHelpersAndEnums.hpp"
#include "nvrhi/nvrhi.h"

namespace Dimensional {

Ref<Shader> Shader::Create(const std::string& path, const ShaderCreateInfo info)
{

    std::filesystem::path fs_path(path);

    Ref<Shader> shader = Ref<Shader>(new Shader());
    shader->m_Name = fs_path.filename();

    if (!shader->Compile(path, info)) {
        DM_CORE_ERROR("FAILED TO COMPILE SHADER: {}", shader->m_Name);
        return nullptr;
    }
    if (!shader->createLayouts()) {
        DM_CORE_ERROR("FAILED TO CREATE LAYOUTS FOR SHADER: {}", shader->m_Name);
        return nullptr;
    }

    return shader;
}

bool Shader::Compile(std::string path, const ShaderCreateInfo info)
{
    ShaderCompiler sc;
    ShaderCompileOptions options;
    options.definesMacros = info.defines;
    options.includePaths = info.includePaths;
    options.optimizationLevel = info.optimizationLevel;

    m_Shaders = sc.compileAllEntryPoints(Application::getDeviceManager()->GetDevice(), path, options);

    return true;
}

void Shader::retrieveResources()
{

    std::map<u32, std::map<u32, ShaderResource>> out; // space/set => (Binding slot -> Resource)

    for (const auto& [type, variant] : m_Shaders) {
        auto& resources = variant.reflection.resources;

        for (const auto& resource : resources) {
            if (out[resource.binding.space].contains(resource.binding.slot)) {
                DM_CORE_WARN("Shader, {0}, contains duplicate bindings at binding space/slot: {1}/{2} [Replacing]", m_Name, resource.binding.space, resource.binding.slot)
            }
            out[resource.binding.space][resource.binding.slot] = resource;
        }
    }

    m_Resources = out;
}

bool Shader::createLayouts()
{
    retrieveResources();

    // SRV Readonly UAV ReadWrite
    for (auto& [space, resources] : m_Resources) {
        nvrhi::BindingLayoutDesc lDesc;
        lDesc.setRegisterSpaceIsDescriptorSet(true);
        lDesc.setRegisterSpace(space);
        lDesc.setVisibility(nvrhi::ShaderType::All);

        for (auto& [slot, resource] : resources) {
            bool canWrite = resource.access == ShaderResourceAccess::READ_WRITE || resource.access == ShaderResourceAccess::WRITE;

            nvrhi::BindingLayoutItem item;

            switch (resource.kind) {

            case ShaderResourceKind::ConstantBuffer:
                item = nvrhi::BindingLayoutItem::ConstantBuffer(resource.binding.slot);
            case ShaderResourceKind::StructuredBuffer:
                if (canWrite) {
                    item = nvrhi::BindingLayoutItem::StructuredBuffer_UAV(resource.binding.slot);
                } else {
                    item = nvrhi::BindingLayoutItem::StructuredBuffer_SRV(resource.binding.slot);
                }
                break;

            case ShaderResourceKind::Texture1DArray:
            case ShaderResourceKind::Texture2DArray:
            case ShaderResourceKind::Texture1D:
            case ShaderResourceKind::Texture2D:
            case ShaderResourceKind::Texture3D:
            case ShaderResourceKind::TextureCube:
            case ShaderResourceKind::TextureBuffer:
                if (canWrite) {
                    item = nvrhi::BindingLayoutItem::Texture_UAV(resource.binding.slot);
                } else {
                    item = nvrhi::BindingLayoutItem::Texture_SRV(resource.binding.slot);
                }
                break;

            case ShaderResourceKind::Sampler:
                item = nvrhi::BindingLayoutItem::Sampler(resource.binding.slot);
                break;
            case ShaderResourceKind::ByteBuffer:
                if (canWrite) {
                    item = nvrhi::BindingLayoutItem::RawBuffer_UAV(resource.binding.slot);
                } else {
                    item = nvrhi::BindingLayoutItem::RawBuffer_SRV(resource.binding.slot);
                }
                break;
            case ShaderResourceKind::Unknown:
                break;
            }
            lDesc.addItem(item);
        }
        auto dev = Application::getDevice();
        nvrhi::BindingLayoutHandle layout = dev->createBindingLayout(lDesc);
        if (!layout) {
            DM_CORE_ERROR("Failed to create binding layout: {0}, {1}", m_Name, space);
            return false;
        }
        m_Layouts[space] = layout;
    }
    return true;
}

nvrhi::ShaderHandle Shader::GetShaderHandle(nvrhi::ShaderType type)
{
    if (!m_Shaders.contains(type)) {
        DM_CORE_WARN("Tried to get shader handle of type {} when it doesnt exists", ShaderTypeToString(type))
    }
    return m_Shaders.at(type).handle;
}
const ShaderVarient& Shader::GetShaderVariant(nvrhi::ShaderType type)
{
    if (!m_Shaders.contains(type)) {
        DM_CORE_WARN("Tried to get shader variant of type {} when it doesnt exists", ShaderTypeToString(type))
    }
    return m_Shaders.at(type);
}

}
