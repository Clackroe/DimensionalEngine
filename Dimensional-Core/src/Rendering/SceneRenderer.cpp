#include "Rendering/SceneRenderer.hpp"
#include "Rendering/GPUBuffer.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Dimensional {

SceneRenderer::SceneRenderer(Ref<Scene> sc)
{
    m_Scene = sc;

    GPUBufferData data;
    data.usage = GPUBufferUsage::DYNAMIC;
    data.type = GPUBufferType::UNIFORM;
    data.sizeBytes = sizeof(CameraData);
    data.data = 0;
    data.slot = 0;
    m_CameraBuffer = GPUBuffer::Create(data);

    //====

    Temp t;

    glm::mat4 m = glm::identity<glm::mat4>();
    m = glm::translate(m, glm::vec3(0, 0, 0));
    m = glm::scale(m, glm::vec3(10, 10, 1));
    t.model = m;

    GPUBufferData data2;
    data2.usage = GPUBufferUsage::DYNAMIC;
    data2.type = GPUBufferType::UNIFORM;
    data2.sizeBytes = sizeof(Temp);
    data2.data = &m;
    data2.slot = 1;
    m_Temp = GPUBuffer::Create(data2);
}

void SceneRenderer::BeginFrame(const CameraData& camData)
{
    m_CameraBuffer->SetData(&camData, 0, sizeof(camData));
}

void SceneRenderer::EndFrame()
{
}
}
