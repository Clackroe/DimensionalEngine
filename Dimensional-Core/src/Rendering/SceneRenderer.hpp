#ifndef DM_SCENERENDERER_HPP
#define DM_SCENERENDERER_HPP

#include "Rendering/EnvironmentMap.hpp"
#include "Rendering/FrameBuffer.hpp"
#include "Rendering/UniformBuffer.hpp"
#include <Scene/Scene.hpp>
#include <buffer.hpp>

namespace Dimensional {
#define MAX_SPOTLIGHTS 100
#define MAX_POINTLIGHTS 100

struct alignas(16) CameraData {
    glm::mat4 viewProj;
    glm::vec3 uCameraPosition;
    float p1;
    glm::mat4 view;
    glm::mat4 proj;
};

struct alignas(16) LightData {
    glm::vec3 position;
    float p1;
    glm::vec3 direction;
    float p2;
    glm::vec3 color;

    float cutOff;
    float outerCutOff;

    float intensity;
    float constant;
    float linear;
    float quadratic;
};

struct EnvironmentData {
    Ref<EnvironmentMap> envMap;
    float lod = 1.0f;
};

class SceneRenderer {
public:
    SceneRenderer() = default;
    SceneRenderer(Ref<Scene> scene)
        : m_Scene(scene)
    {
        m_CameraUBO = CreateRef<UniformBuffer>(sizeof(CameraData), 0);
        m_PointLightUBO = CreateRef<UniformBuffer>(MAX_POINTLIGHTS * sizeof(LightData) + sizeof(int), 1);
        m_SpotLightUBO = CreateRef<UniformBuffer>(MAX_SPOTLIGHTS * sizeof(LightData) + sizeof(int), 2);
        FrameBufferSettings fbs = {
            1920,
            1080,
            { RGBA32F, Depth }
        };
        m_FrameBuffer = CreateRef<FrameBuffer>(fbs);
        m_CubeMapShader = CreateRef<Shader>("Assets/Shaders/CubeMap.glsl");
    }

    void beginScene(CameraData camData);
    void endScene();

    void render();

    Ref<FrameBuffer> getFrameBuffer() { return m_FrameBuffer; };

private:
    void setupLightData();
    void setupCameraData();

private:
    Ref<Scene> m_Scene;

    std::vector<LightData> m_PointLightData;
    std::vector<LightData> m_SpotLightData;

    CameraData m_CameraData;

    Ref<UniformBuffer> m_PointLightUBO;
    Ref<UniformBuffer> m_SpotLightUBO;

    Ref<UniformBuffer> m_CameraUBO;

    Ref<Shader> m_CubeMapShader;
    EnvironmentData m_CurrentEnvironmentMap;

    Ref<FrameBuffer> m_FrameBuffer;
};

}

#endif
