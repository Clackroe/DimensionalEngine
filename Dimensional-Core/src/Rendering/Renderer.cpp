#include "Core/Application.hpp"
#include "Log/log.hpp"
#include "Rendering/Samplers.hpp"
#include <Rendering/Renderer.hpp>
#include <nvrhi/nvrhi.h>
#include <vector>

namespace Dimensional {

static struct RendererData {
    SamplerCollection samplers;

    nvrhi::BindingLayoutHandle constantBindingLayout;
    nvrhi::BindingSetHandle constantBindingSet;
} s_RendererInstance;

static nvrhi::BindingLayoutHandle createConstantBindingLayout(nvrhi::DeviceHandle dev, std::map<u32, nvrhi::BindingLayoutItem> items)
{
    nvrhi::BindingLayoutDesc desc;
    desc.setRegisterSpaceIsDescriptorSet(true);
    desc.setRegisterSpace((u32)RESOURCE_DOMAIN::CONSTANT);
    desc.setVisibility(nvrhi::ShaderType::All);

    for (auto& [slot, it] : items) {
        desc.addItem(it);
    }

    auto out = dev->createBindingLayout(desc);
    if (!out) {
        DM_CORE_WARN("Failed to create Constant Binding layout for Renderer");
        return nullptr;
    }
    return out;
}

static nvrhi::BindingSetHandle createConstantBindingSet(nvrhi::DeviceHandle dev, std::map<u32, nvrhi::BindingSetItem> items, nvrhi::BindingLayoutHandle layout)
{
    nvrhi::BindingSetDesc desc;
    // desc.setTrackLiveness(true);

    for (auto& [slot, it] : items) {
        desc.addItem(it);
    }

    auto out = dev->createBindingSet(desc, layout);
    if (!out) {
        DM_CORE_WARN("Failed to create Constant Binding Set for Renderer");
        return nullptr;
    }
    return out;
}

#define SAMPLER_ITEM_SET(pair)                                                                \
    bindingSetItems[pair.second] = (nvrhi::BindingSetItem::Sampler(pair.second, pair.first)); \
    bindingLayoutItems[pair.second] = (nvrhi::BindingLayoutItem::Sampler(pair.second));

void Renderer::Init(const RendererInfo& data)
{

    std::map<u32, nvrhi::BindingLayoutItem> bindingLayoutItems;
    std::map<u32, nvrhi::BindingSetItem> bindingSetItems;

    auto dev = Application::getDevice();
    s_RendererInstance.samplers = CreateAllSamplers(dev);

    SAMPLER_ITEM_SET(s_RendererInstance.samplers.anisotropicClamp)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.anisotropicRepeat)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.linearClamp)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.linearRepeat)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.linearComparison)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.linearMirror)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.pointComparison)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.pointMirror)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.pointClamp)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.pointRepeat)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.blackBorder)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.whiteBorder)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.maxReduction)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.minReduction)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.noise)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.skybox)
    SAMPLER_ITEM_SET(s_RendererInstance.samplers.ui)

    s_RendererInstance.constantBindingLayout = createConstantBindingLayout(dev, bindingLayoutItems);
    s_RendererInstance.constantBindingSet = createConstantBindingSet(dev, bindingSetItems, s_RendererInstance.constantBindingLayout);
}
#undef SAMPLER_ITEM_SET

nvrhi::BindingLayoutHandle Renderer::GetConstantBindingLayout()
{
    if (!s_RendererInstance.constantBindingLayout) {
        DM_CORE_WARN("Tried to get constant binding layout from renderer, when it is null")
        return nullptr;
    }
    return s_RendererInstance.constantBindingLayout;
}
nvrhi::BindingSetHandle Renderer::GetConstantBindingSet()
{
    if (!s_RendererInstance.constantBindingSet) {
        DM_CORE_WARN("Tried to get constant binding set from renderer, when it is null")
        return nullptr;
    }
    return s_RendererInstance.constantBindingSet;
}

// void Renderer::ApplyConstantState(nvrhi::GraphicsState& state)
// {
//     auto set = GetConstantBindingSet();
//     if (set) {
//         state.addBindingSet(set);
//     }
// }

nvrhi::GraphicsState Renderer::GetDefaultGraphicsState()
{
    nvrhi::GraphicsState state;
    auto set = GetConstantBindingSet();
    state.addBindingSet(set);

    return state;
}

void Renderer::Shutdown()
{
}

}
