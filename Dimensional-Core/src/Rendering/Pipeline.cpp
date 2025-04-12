

#include "Rendering/Pipeline.hpp"
#include "Log/log.hpp"
#include "nvrhi/nvrhi.h"
#include <Core/Application.hpp>
namespace Dimensional {
Ref<Pipeline> Pipeline::Create(const PipelineCreateInfo& info)
{
    Ref<Pipeline> pipe = Ref<Pipeline>(new Pipeline);
    pipe->m_Shader = info.shader;
    pipe->m_Name = info.debugName;
    return pipe;
}

void Pipeline::SetInput(u32 set, u32 binding, const nvrhi::ResourceHandle res)
{
    // Ensure we have enough binding layouts
    if (set >= m_Shader->GetBindingLayouts().size()) {
        DM_CORE_ERROR("Pipeline: {0} Set {1} exceeds available binding layouts", m_Name, set);
        return;
    }

    auto bindingLayout = m_Shader->GetBindingLayouts()[set];
    if (!bindingLayout) {
        DM_CORE_ERROR("Pipeline: {0} No binding layout for set {1}", m_Name, set);
        return;
    }

    // Find the binding in the layout
    const auto& layoutDesc = *bindingLayout->getDesc();
    auto it = std::find_if(layoutDesc.bindings.begin(), layoutDesc.bindings.end(),
        [binding](const auto& item) {
            return item.slot == binding;
        });

    if (it == layoutDesc.bindings.end()) {
        DM_CORE_WARN("Pipeline: {0} No binding slot {1} in set {2}", m_Name, binding, set);
        return;
    }

    nvrhi::BindingSetItem item;
    item.slot = binding;
    item.type = it->type; // Use the type from the layout
    item.resourceHandle = res;
    item.format = nvrhi::Format::UNKNOWN;
    item.dimension = nvrhi::TextureDimension::Unknown;
    item.subresources = nvrhi::AllSubresources;
    item.unused = 0;

    m_BindingSetItemBySet[set].push_back(item);
    // m_DirtyBindingSets = true; // Mark for rebinding
}

bool Pipeline::Compile(nvrhi::FramebufferHandle tempFramebuffer)
{
    if (!SetupBindingSet()) {
        return false;
    }
    if (!CreatePipeline(tempFramebuffer)) {
        return false;
    }

    return true;
}

nvrhi::GraphicsState Pipeline::Bind(nvrhi::GraphicsState state)
{
    state.setPipeline(m_Pipeline);
    for (size_t i = 0; i < m_BindingSets.size(); ++i) {
        if (!m_BindingSets[i]) {
            DM_CORE_ERROR("Pipleline: {0} Binding set at index {1} is null!", m_Name, i);
        }
        state.addBindingSet(m_BindingSets[i]);
    }

    return state;
}

bool Pipeline::CreatePipeline(nvrhi::FramebufferHandle tempFramebuffer)
{
    auto pipelineDesc = nvrhi::GraphicsPipelineDesc()
                            .setInputLayout(m_Shader->GetInputLayout())
                            .setVertexShader(m_Shader->GetShaderHandle(ShaderType::VERTEX))
                            .setPixelShader(m_Shader->GetShaderHandle(ShaderType::FRAGMENT));
    pipelineDesc.primType = nvrhi::PrimitiveType::TriangleList;
    pipelineDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
    pipelineDesc.renderState.depthStencilState.depthTestEnable = false;

    for (auto& layout : m_Shader->GetBindingLayouts()) {
        pipelineDesc.addBindingLayout(layout);
    }

    m_Pipeline = Application::getDevice()->createGraphicsPipeline(pipelineDesc, tempFramebuffer);
    if (!m_Pipeline) {
        DM_CORE_ERROR("Failed to create Pipeline {}", m_Name)
        return false;
    }
    return true;
}

bool Pipeline::SetupBindingSet()
{
    m_BindingSets.clear();
    const auto& bindingLayouts = m_Shader->GetBindingLayouts();

    for (size_t setIndex = 0; setIndex < bindingLayouts.size(); setIndex++) {
        auto& layout = bindingLayouts[setIndex];
        nvrhi::BindingLayoutDesc layoutDesc = *layout->getDesc();
        nvrhi::BindingSetDesc desc;

        // Skip if no binding layout is defined for this set
        if (!layout) {
            DM_CORE_WARN("Pipeline: {0} No binding layout for set {1}", m_Name, setIndex);
            continue;
        }

        // If we have provided bindings for this set
        if (m_BindingSetItemBySet.contains(setIndex)) {
            auto& setItems = m_BindingSetItemBySet[setIndex];

            // Add each item from the layout if we have a corresponding binding
            for (const auto& layoutItem : layoutDesc.bindings) {

                auto it = std::find_if(setItems.begin(), setItems.end(),
                    [&layoutItem](const auto& item) {
                        return item.slot == layoutItem.slot;
                    });

                if (it != setItems.end()) {
                    if (it->type != layoutItem.type) {
                        DM_CORE_ERROR("Pipeline: {0} Type mismatch for binding {1} in set {2}",
                            m_Name, layoutItem.slot, setIndex);
                        return false;
                    }
                    desc.addItem(*it);
                } else {
                    DM_CORE_WARN("Pipeline: {0} Missing binding for slot {1} in set {2}",
                        m_Name, layoutItem.slot, setIndex);
                    // TODO: default binding?
                }
            }
        } else {
            // DM_CORE_WARN("Pipeline: {0} No binding items provided for set {1}", m_Name, setIndex);
            // TODO Handle differently? - empty set or default values?
        }

        auto bs = Application::getDevice()->createBindingSet(desc, layout);
        if (!bs) {
            DM_CORE_ERROR("Failed to create Binding Set on Pipeline {0}: Set - {1}", m_Name, setIndex);
            return false;
        }

        m_BindingSets.push_back(bs);
    }

    return true;
}

}
