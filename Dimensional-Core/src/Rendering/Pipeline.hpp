#ifndef DM_PIPELINE_HPP
#define DM_PIPELINE_HPP

#include "Rendering/Shader.hpp"
#include "nvrhi/nvrhi.h"
namespace Dimensional {

struct PipelineCreateinfo {
    Ref<Shader> shader;
    nvrhi::PrimitiveType primitive = nvrhi::PrimitiveType::TriangleList;
};

class GraphicsPipeline {
public:
    static Ref<GraphicsPipeline> Create(const PipelineCreateinfo);

private:
    GraphicsPipeline() = default;
};

}
#endif // DM_PIPELINE_HPP
