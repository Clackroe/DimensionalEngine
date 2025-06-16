#ifndef DM_PIPELINE_HPP
#define DM_PIPELINE_HPP

#include "Rendering/Shader.hpp"
#include "Rendering/ShaderReflection.hpp"
#include "nvrhi/nvrhi.h"
namespace Dimensional {

struct GraphicsPipelineCreateinfo {
    Ref<Shader> shader;
    nvrhi::PrimitiveType primitive = nvrhi::PrimitiveType::TriangleList;
    std::string debugName = "Dimensional Pipeline";

    // TEMPORARY
    nvrhi::FramebufferHandle TEMPframebuff;
    //===
};

class GraphicsPipeline {
public:
    static Ref<GraphicsPipeline> Create(const GraphicsPipelineCreateinfo info);

    nvrhi::GraphicsPipelineHandle getPipeline() { return m_Pipeline; };

    void SetConstantSpace(nvrhi::BindingSetHandle handle);
    void SetPerFrameSpace(nvrhi::BindingSetHandle handle);

    // TODO: Implement Materials (Add Material instead of handle)
    void SetMaterial(nvrhi::BindingSetHandle handle);

    void SetTexture(nvrhi::TextureHandle handle, u32 slot);

    void Bind(nvrhi::GraphicsState& state);

    bool Compile();

private:
    // TEMPORARY
    nvrhi::FramebufferHandle m_TEMPframebuff;
    //===

    GraphicsPipeline() = default;

    bool createPipelineBindingSet();

    bool createNVRHIPipeline();

    std::map<u32, nvrhi::BindingSetHandle> m_BindingSets; // space => BindingSet
    std::map<u32, nvrhi::BindingSetItem> m_PipelineSetItems; // Slot-> setItem

    nvrhi::GraphicsPipelineHandle m_Pipeline;

    Ref<Shader> m_Shader;
    nvrhi::PrimitiveType m_PrimType = nvrhi::PrimitiveType::TriangleList;

    bool m_PipeLineSetDirty = false;
    bool m_ShouldRecompile = false;

    std::string m_Name;
};

}
#endif // DM_PIPELINE_HPP
