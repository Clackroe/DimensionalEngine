#ifndef DM_SCENERENDERER_HPP
#define DM_SCENERENDERER_HPP

#include "Rendering/EnvironmentMap.hpp"
#include "Rendering/FrameBuffer.hpp"
#include "Rendering/UniformBuffer.hpp"
#include <Scene/Scene.hpp>
#include <buffer.hpp>

namespace Dimensional {
#define MAX_POINTLIGHTS 300

struct alignas(16) CameraData {
    glm::mat4 viewProj;
    glm::vec3 uCameraPosition;
    float p1;
    glm::mat4 view;
    glm::mat4 proj;
};

struct alignas(16) DirectionalLightData {
    glm::mat4 viewProj;
};

struct alignas(16) LightData {
    glm::vec4 position;
    glm::vec4 direction;

    // ([color], intensity)
    glm::vec4 color;

    // cuttoff, outerCutoff, radius, type (0 pLight, 1 Spotlight)
    glm::vec4 params;
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
        FrameBufferSettings fbs = {
            1920,
            1080,
            { RGBA32F, Depth }
        };
        m_FrameBuffer = CreateRef<FrameBuffer>(fbs);

        FrameBufferSettings dfbs = {
            2048,
            2048,
            { Shadow }
        };
        m_DirLightFB = CreateRef<FrameBuffer>(dfbs);
        m_ShadowMapShader = CreateRef<Shader>("Assets/Shaders/ShadowMap.glsl");
        m_DirLightUBO = CreateRef<UniformBuffer>(sizeof(DirectionalLightData), 2);

        m_CubeMapShader = CreateRef<Shader>("Assets/Shaders/CubeMap.glsl");
    }

    void beginScene(CameraData camData);
    void endScene();

    void render();

    Ref<FrameBuffer> getFrameBuffer() { return m_FrameBuffer; };
    Ref<FrameBuffer> getFrameBufferTEMP() { return m_DirLightFB; };

private:
    void setupLightData();
    void setupCameraData();

    void shadowPass();

private:
    Ref<Scene> m_Scene;

    std::vector<LightData> m_PointLightData;

    CameraData m_CameraData;

    Ref<UniformBuffer> m_PointLightUBO;

    Ref<UniformBuffer> m_CameraUBO;

    Ref<Shader> m_CubeMapShader;
    EnvironmentData m_CurrentEnvironmentMap;

    DirectionalLightData m_DirLightData;
    Ref<FrameBuffer> m_DirLightFB;
    Ref<Shader> m_ShadowMapShader;
    u32 m_DirLightDepthID;
    Ref<UniformBuffer> m_DirLightUBO;

    Ref<FrameBuffer> m_FrameBuffer;
};

}

#endif
