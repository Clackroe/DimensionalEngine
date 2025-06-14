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

private:
    // TEMPORARY
    nvrhi::FramebufferHandle m_TEMPframebuff;
    //===

    GraphicsPipeline() = default;

    void retrievePipelineBindings();
    bool createBindingLayout();

    bool createNVRHIPipeline();

    nvrhi::BindingLayoutHandle m_PipelineLayout;
    nvrhi::GraphicsPipelineHandle m_Pipeline;

    Ref<Shader> m_Shader;
    nvrhi::PrimitiveType m_PrimType = nvrhi::PrimitiveType::TriangleList;

    std::map<u32, ShaderResource> m_Resources; // Binding slot -> Resource
    std::string m_Name;
};

}
#endif // DM_PIPELINE_HPP
