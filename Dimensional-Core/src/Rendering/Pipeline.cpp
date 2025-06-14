#include "Core/Application.hpp"
#include "Log/log.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderCompiler.hpp"
#include "Rendering/ShaderReflection.hpp"
#include "nvrhi/nvrhi.h"
#include <Rendering/Pipeline.hpp>
#include <vector>

namespace Dimensional {

Ref<GraphicsPipeline> GraphicsPipeline::Create(const GraphicsPipelineCreateinfo info)
{
    Ref<GraphicsPipeline> out = Ref<GraphicsPipeline>(new GraphicsPipeline());
    out->m_Name = info.debugName;
    out->m_Shader = info.shader;
    out->m_TEMPframebuff = info.TEMPframebuff;

    if (!out->createBindingLayout()) {
        DM_CORE_ERROR("Failed to create Pipeline: {}", out->m_Name);
        return nullptr;
    }

    return out;
}

bool GraphicsPipeline::createNVRHIPipeline()
{
    nvrhi::GraphicsPipelineDesc desc;
    desc.setVertexShader(m_Shader->GetShaderHandle(nvrhi::ShaderType::Vertex));
    desc.setFragmentShader(m_Shader->GetShaderHandle(nvrhi::ShaderType::Pixel));
    desc.setPrimType(m_PrimType);
    desc.addBindingLayout(m_PipelineLayout);

    auto dev = Application::getDevice();

    nvrhi::GraphicsPipelineHandle pipe = dev->createGraphicsPipeline(desc, m_TEMPframebuff);
    if (!pipe) {
        DM_CORE_ERROR("Failed to create Pipeline: {}", m_Name);
        return false;
    }
    m_Pipeline = pipe;

    return true;
}

void GraphicsPipeline::retrievePipelineBindings()
{

    std::map<u32, ShaderResource> out; // Specifically, the pipeline sets | binding slot -> Resource

    for (const auto& [type, variant] : m_Shader->m_Shaders) {
        auto& resources = variant.reflection.resources;

        for (const auto& resource : resources) {
            if (resource.binding.slot == (u32)RESOURCE_DOMAIN::PIPELINE) {
                if (out.contains(resource.binding.slot)) {
                    DM_CORE_WARN("Pipeline, {0}, contains duplicate bindings at binding slot: {1} [Replacing]", m_Name, resource.binding.slot)
                }
                out[resource.binding.slot] = resource;
            }
        }
    }

    m_Resources = out;
}

bool GraphicsPipeline::createBindingLayout()
{
    retrievePipelineBindings();

    nvrhi::BindingLayoutDesc lDesc;
    lDesc.setRegisterSpaceIsDescriptorSet(true);
    lDesc.setRegisterSpace((u32)RESOURCE_DOMAIN::PIPELINE);
    lDesc.visibility = nvrhi::ShaderType::AllGraphics;

    // SRV Readonly UAV ReadWrite
    for (auto& [_, resource] : m_Resources) {

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

        case ShaderResourceKind::Sampler:
            item = nvrhi::BindingLayoutItem::Sampler(resource.binding.slot);
        case ShaderResourceKind::ByteBuffer:
            if (canWrite) {
                item = nvrhi::BindingLayoutItem::RawBuffer_SRV(resource.binding.slot);
            } else {
                item = nvrhi::BindingLayoutItem::RawBuffer_UAV(resource.binding.slot);
            }
        case ShaderResourceKind::Unknown:
            break;
        }
        lDesc.addItem(item);
    }
    auto dev = Application::getDevice();
    m_PipelineLayout = dev->createBindingLayout(lDesc);
    if (!m_PipelineLayout) {
        DM_CORE_ERROR("Failed to create Pipeline binding layout: {}", m_Name);
        return false;
    }
    return true;
}
}
