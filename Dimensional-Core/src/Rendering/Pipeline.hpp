#ifndef DM_PIPELINE_HPP
#define DM_PIPELINE_HPP
#include "Rendering/Shader.hpp"
#include "nvrhi/nvrhi.h"
namespace Dimensional {

struct PipelineCreateInfo {
    Ref<Shader> shader;
    std::string debugName = "Untitled Pipeline";
    // Culling, Depth funcs/Test, etc
    //
    // RenderTarget
};

struct Pipeline {

    static Ref<Pipeline> Create(const PipelineCreateInfo& info);

    void SetInput(u32 set, u32 binding, const nvrhi::ResourceHandle res);

    nvrhi::GraphicsState Bind(nvrhi::GraphicsState state);

    bool Compile(nvrhi::FramebufferHandle tempFramebuffer);

private:
    Pipeline() = default;
    Ref<Shader> m_Shader;

    nvrhi::GraphicsPipelineHandle m_Pipeline = nullptr;

    bool SetupBindingSet();
    bool CreatePipeline(nvrhi::FramebufferHandle tempFramebuffer);

    std::vector<nvrhi::BindingSetHandle> m_BindingSets;

    std::map<u32, std::vector<nvrhi::BindingSetItem>> m_BindingSetItemBySet;

    std::string m_Name;
};
}
#endif // DM_PIPELINE_HPP
