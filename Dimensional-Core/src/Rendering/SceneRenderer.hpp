#ifndef SCENE_RENDERER_HPP
#define SCENE_RENDERER_HPP

#include "Rendering/GPUBuffer.hpp"
#include "Scene/Scene.hpp"
namespace Dimensional {

struct alignas(16) CameraData {
    glm::mat4 viewProj;
    glm::vec3 uCameraPosition;
    float aspectRatio;
    glm::mat4 view;
    glm::mat4 proj;
    float fov;
    float nearPlane;
    float farPlane;
};

struct alignas(16) Temp {
    glm::mat4 model;
};

struct SceneRenderer {

    SceneRenderer(Ref<Scene> sc);

    void BeginFrame(const CameraData& camData);

    void EndFrame();

private:
    Ref<Scene> m_Scene;

    Ref<GPUBuffer> m_CameraBuffer;

    Ref<GPUBuffer> m_Temp;
};

}

#endif // SCENE_RENDERER_HPP
