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

    return out;
}

void GraphicsPipeline::SetConstantSpace(nvrhi::BindingSetHandle handle)
{
    m_BindingSets[(u32)RESOURCE_DOMAIN::CONSTANT] = handle;
}
void GraphicsPipeline::SetPerFrameSpace(nvrhi::BindingSetHandle handle)
{
    m_BindingSets[(u32)RESOURCE_DOMAIN::FRAME] = handle;
}
void GraphicsPipeline::SetMaterial(nvrhi::BindingSetHandle handle)
{
    m_BindingSets[(u32)RESOURCE_DOMAIN::MATERIAL] = handle;
}

void GraphicsPipeline::SetTexture(Ref<Texture2D> tex, u32 slot)
{
    m_PipeLineSetDirty = true;
    m_PipelineSetItems[slot] = (nvrhi::BindingSetItem::Texture_SRV(slot, tex->GetHandle()));
}
void GraphicsPipeline::SetTextureUAV(Ref<Texture2D> tex, u32 slot)
{
    m_PipeLineSetDirty = true;
    m_PipelineSetItems[slot] = (nvrhi::BindingSetItem::Texture_UAV(slot, tex->GetHandle()));
}

void GraphicsPipeline::SetTexture(nvrhi::TextureHandle handle, u32 slot)
{
    m_PipeLineSetDirty = true;
    m_PipelineSetItems[slot] = (nvrhi::BindingSetItem::Texture_SRV(slot, handle));
}

bool GraphicsPipeline::Compile()
{
    if (m_PipeLineSetDirty) {
        if (!createPipelineBindingSet()) {
            return false;
        }
    }
    if (!createNVRHIPipeline()) {
        return false;
    }

    return true;
}

bool GraphicsPipeline::createPipelineBindingSet()
{

    nvrhi::BindingSetDesc desc;
    for (auto& [slot, item] : m_PipelineSetItems) {
        desc.addItem(item);
    }

    auto dev = Application::getDevice();

    if (m_Shader->m_Layouts.contains((u32)RESOURCE_DOMAIN::PIPELINE)) {
        auto set = dev->createBindingSet(desc, m_Shader->m_Layouts.at((u32)RESOURCE_DOMAIN::PIPELINE));
        if (!set) {
            DM_CORE_ERROR("Failed to create Pipeline Binding set for pipeline: {}", m_Name);
            return false;
        }
        m_BindingSets[(u32)RESOURCE_DOMAIN::PIPELINE] = set;
        return true;
    }
    DM_CORE_ERROR("Pipeline Binding set doesn't exist for pipeline: {}", m_Name);
    return false;
}

void GraphicsPipeline::Bind(nvrhi::GraphicsState& state)
{
    state.setPipeline(m_Pipeline);

    // TODO: Set Framebuffer (once rendertarges are implemented)

    // Maintain parity with binding layouts
    for (auto& [set, layout] : m_Shader->m_Layouts) {
        if (m_BindingSets.contains(set)) {
            state.addBindingSet(m_BindingSets[set]);
        } else {
            state.addBindingSet(nullptr);
        }
    }
}

bool GraphicsPipeline::createNVRHIPipeline()
{
    nvrhi::GraphicsPipelineDesc desc;
    desc.setInputLayout(m_Shader->GetShaderVariant(nvrhi::ShaderType::Vertex).inputLayout);
    desc.setVertexShader(m_Shader->GetShaderHandle(nvrhi::ShaderType::Vertex));
    desc.setFragmentShader(m_Shader->GetShaderHandle(nvrhi::ShaderType::Pixel));
    desc.setPrimType(m_PrimType);

    // TODO: Make more modular and configurable;
    desc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
    desc.renderState.depthStencilState.depthTestEnable = false;
    //

    for (auto& [_space, set] : m_BindingSets) {
        desc.addBindingLayout(set->getLayout());
    }

    auto dev = Application::getDevice();

    nvrhi::GraphicsPipelineHandle pipe = dev->createGraphicsPipeline(desc, m_TEMPframebuff);
    if (!pipe) {
        DM_CORE_ERROR("Failed to create Pipeline: {}", m_Name);
        return false;
    }
    m_Pipeline = pipe;

    return true;
}

}
