

#include "Rendering/Pipeline.hpp"
#include "Log/log.hpp"
#include "nvrhi/nvrhi.h"
#include <Core/Application.hpp>
namespace Dimensional {
Ref<Pipeline> Pipeline::Create(const PipelineCreateInfo& info)
{
    Ref<Pipeline> pipe = Ref<Pipeline>(new Pipeline);
    pipe->m_Shader = info.shader;
    return pipe;
}

void Pipeline::SetInput(u32 set, u32 binding, const nvrhi::ResourceHandle res)
{

    auto& expectedType = m_Shader->GetBindingLayouts()[set]->getDesc()->bindings[binding].type;

    nvrhi::BindingSetItem item;
    item.slot = binding;
    item.type = expectedType;
    item.resourceHandle = res;
    item.format = nvrhi::Format::UNKNOWN;
    item.dimension = nvrhi::TextureDimension::Unknown;
    item.subresources = nvrhi::AllSubresources;
    item.unused = 0;

    m_BindingSetItemBySet[set].push_back(item);
    // Should probably Flag the dirty flag to ensure this gets bound properly
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
            DM_CORE_ERROR("Binding set at index {} is null!", i);
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
        DM_CORE_ERROR("Failed to create Pipeline")
        return false;
    }
    return true;
}

bool Pipeline::SetupBindingSet()
{
    m_BindingSets.clear();

    // Get binding layouts from shader
    const auto& bindingLayouts = m_Shader->GetBindingLayouts();

    // Process each binding layout
    for (size_t setIndex = 0; setIndex < bindingLayouts.size(); setIndex++) {
        auto& layout = bindingLayouts[setIndex];
        nvrhi::BindingLayoutDesc layoutDesc = *layout->getDesc();

        // Skip if we don't have items for this set
        if (!m_BindingSetItemBySet.contains(setIndex)) {
            DM_CORE_WARN("No binding items for set {} in shader", setIndex);
            return false;
        }

        // Get items for this set and sort by binding slot
        auto& setItems = m_BindingSetItemBySet[setIndex];
        std::stable_sort(setItems.begin(), setItems.end(), [](const auto& a, const auto& b) {
            return a.slot < b.slot;
        });

        // Validate items against layout
        if (setItems.size() != layoutDesc.bindings.size()) {
            DM_CORE_ERROR("Binding count mismatch for set {}: layout has {}, provided {}",
                setIndex, layoutDesc.bindings.size(), setItems.size());
            return false;
        }

        // Create binding set descriptor
        nvrhi::BindingSetDesc desc;

        // Add items in the same order as the layout
        std::vector<nvrhi::BindingSetItem> orderedItems;
        for (size_t i = 0; i < layoutDesc.bindings.size(); i++) {
            const auto& layoutItem = layoutDesc.bindings[i];

            // Find the corresponding item with the same slot
            auto it = std::find_if(setItems.begin(), setItems.end(),
                [&layoutItem](const auto& item) {
                    return item.slot == layoutItem.slot;
                });

            if (it == setItems.end()) {
                DM_CORE_ERROR("Missing binding for slot {} in set {}", layoutItem.slot, setIndex);
                return false;
            }

            // Check type compatibility
            if (it->type != layoutItem.type) {
                DM_CORE_ERROR("Type mismatch for binding {} in set {}: expected {}, got {}",
                    layoutItem.slot, setIndex,
                    static_cast<int>(layoutItem.type), static_cast<int>(it->type));
                return false;
            }

            orderedItems.push_back(*it);
        }

        // Set ordered items in descriptor
        for (auto& i : orderedItems) {
            desc.addItem(i);
        }

        // Create binding set
        auto bs = Application::getDevice()->createBindingSet(desc, layout);
        if (!bs) {
            DM_CORE_ERROR("Failed to create Binding Set on Pipeline: Set - {}", setIndex);
            return false;
        }

        // Store the binding set
        m_BindingSets.push_back(bs);
    }

    return true;
}

// bool Pipeline::SetupBindingSet()
// {
//
//     m_BindingSets.clear();
//     const auto& bindingLayouts = m_Shader->GetBindingLayouts();
//
//     std::vector<std::vector<nvrhi::BindingSetItem>> sortedItems;
//     u32 maxSet = 0;
//     for (auto& [set, items] : m_BindingSetItemBySet) {
//         maxSet = std::max(maxSet, set);
//     }
//     sortedItems.resize(maxSet + 1);
//
//     for (auto& [set, items] : m_BindingSetItemBySet) {
//         std::stable_sort(items.begin(), items.end(), [](const auto& a, const auto& b) {
//             return a.slot < b.slot;
//         });
//         sortedItems[set] = (items);
//     }
//
//     for (u32 i = 0; i < sortedItems.size(); i++) {
//         auto& items = sortedItems[i];
//         nvrhi::BindingSetDesc desc;
//         for (auto& item : items) {
//             desc.addItem(item);
//         }
//
//         if (m_Shader->GetBindingLayouts().size() > i) {
//             nvrhi::BindingLayoutHandle bl = m_Shader->GetBindingLayouts()[i];
//             nvrhi::BindingSetHandle bs = Application::getDevice()->createBindingSet(desc, bl);
//             if (!bs) {
//                 DM_CORE_WARN("Failed to create Binding Set on Pipeline: Set - {0}", i);
//                 return false;
//             }
//             m_BindingSets.push_back(bs);
//         }
//     }
//
//     return true;
// }

}
