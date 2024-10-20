#include "Log/log.hpp"
#include "Rendering/RendererAPI.hpp"
#include "Rendering/UniformBuffer.hpp"
#include <Rendering/Renderer3d.hpp>
#include <Rendering/SceneRenderer.hpp>
namespace Dimensional {

void SceneRenderer::beginScene(CameraData camData)
{
    m_FrameBuffer->Bind();
    RendererAPI::getInstance().setClearColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
    RendererAPI::getInstance().clearBuffer();

    m_CameraData = camData;
    setupLightData();
    setupCameraData();
}

void SceneRenderer::render()
{
    {
        // Render Meshes
        auto view = m_Scene->m_Registry.view<TransformComponent, MeshRenderer>();
        for (auto e : view) {
            auto [transform, mesh] = view.get<TransformComponent, MeshRenderer>(e);
            Ref<Model> mod = AssetManager::getInstance().getAsset<Model>(mesh.model);
            if (!mod) {
                DM_CORE_WARN("UH OH")
                continue;
            }
            Renderer3D::renderModel(mod, transform.GetTransform());
        }
    }
}

void SceneRenderer::endScene()
{
    m_CubeMapShader->use();
    RendererAPI::getInstance().setDepthFunc(DepthFunc::LEQUAL);
    m_CubeMapShader->setInt("environmentMap", 8);
    if (m_CurrentEnvironmentMap.envMap) {
        m_CurrentEnvironmentMap.envMap->bind();
        m_CubeMapShader->setFloat("uLod", m_CurrentEnvironmentMap.lod);
    }
    Renderer3D::renderCube(m_CubeMapShader);

    // RendererAPI::getInstance().setDepthFunc(DepthFunc::DEFAULT);
    m_FrameBuffer->Unbind();
}

void SceneRenderer::setupCameraData()
{
    m_CameraUBO->setData(&m_CameraData, 0, UBOPaddingInfo<CameraData>::paddedSize);
}
void SceneRenderer::setupLightData()
{

    m_PointLightData.clear();
    m_SpotLightData.clear();

    {
        auto view = m_Scene->m_Registry.view<TransformComponent, PointLightComponent>();
        for (auto e : view) {
            auto [transform, light] = view.get<TransformComponent, PointLightComponent>(e);
            LightData data = {
                transform.Position,
                0.0f,
                transform.Rotation,
                0.0f,
                light.color * 255.0f,
                0.0f,
                0.0f,
                light.intensity,
                light.constant,
                light.linear,
                light.quadratic,
            };
            m_PointLightData.push_back(data);
        }
    }
    {
        auto view = m_Scene->m_Registry.view<TransformComponent, SpotLightComponent>();
        for (auto e : view) {
            auto [transform, light] = view.get<TransformComponent, SpotLightComponent>(e);
            glm::vec3 spotlightDirection = glm::rotate(glm::quat(transform.Rotation), glm::vec3(0.0f, -1.0f, 0.0f));
            LightData data = {
                transform.Position,
                0.0f,
                spotlightDirection,
                0.0f,
                light.color * 255.0f,
                glm::cos(glm::radians(light.cutOff)),
                glm::cos(glm::radians(light.outerCutOff)),
                light.intensity,
                light.constant,
                light.linear,
                light.quadratic,
            };
            m_SpotLightData.push_back(data);
        }
    }

    {
        auto view = m_Scene->m_Registry.view<SkyLight>();
        for (auto l : view) {
            auto envData = view.get<SkyLight>(l);
            Ref<EnvironmentMap> map = AssetManager::getInstance().getAsset<EnvironmentMap>(envData.envMap);
            if (map) {
                m_CurrentEnvironmentMap = { map, envData.lod };
            }
        }
    }

    if (m_CurrentEnvironmentMap.envMap) {
        m_CurrentEnvironmentMap.envMap->bind();
    }

    u32 numPLights = m_PointLightData.size();
    int pLightDataSize = sizeof(LightData) * numPLights;
    int maxPLightSize = sizeof(LightData) * MAX_POINTLIGHTS;

    u32 numSLights = m_SpotLightData.size();
    int sLightDataSize = sizeof(LightData) * numSLights;
    int maxSLightSize = sizeof(LightData) * MAX_SPOTLIGHTS;

    m_PointLightUBO->setData(m_PointLightData.data(), 0, pLightDataSize);
    m_PointLightUBO->setData(&numPLights, maxPLightSize, sizeof(u32));

    m_SpotLightUBO->setData(m_SpotLightData.data(), 0, sLightDataSize);
    m_SpotLightUBO->setData(&numSLights, maxSLightSize, sizeof(u32));
}

}
