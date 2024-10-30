#include "Log/log.hpp"
#include "Rendering/RendererAPI.hpp"
#include "Scene/Components.hpp"
#include <Rendering/Renderer3d.hpp>
#include <Rendering/SceneRenderer.hpp>
namespace Dimensional {

void SceneRenderer::beginScene(CameraData camData)
{
    m_FrameBuffer->Bind();
    RendererAPI::getInstance().setClearColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
    RendererAPI::getInstance().clearBuffer();
    m_FrameBuffer->Unbind();

    m_CameraData = camData;
    setupLightData();
    setupCameraData();
}
void SceneRenderer::shadowPass()
{
    m_DirLightFB->Bind();

    m_ShadowMapShader->use();

    RendererAPI::getInstance().clearBuffer(true);
    for (int index = 0; index < m_DirLightData.size(); index++) {

        m_ShadowMapShader->setInt("uDirLightIndex", index);

        {
            // Render Meshes
            auto view = m_Scene->m_Registry.view<TransformComponent, MeshRenderer>();
            for (auto e : view) {
                auto [transform, mesh] = view.get<TransformComponent, MeshRenderer>(e);
                Ref<Model> mod = AssetManager::getInstance().getAsset<Model>(mesh.model);
                if (!mod) {
                    continue;
                }
                auto& overrides = mesh.materialOverrides;
                Renderer3D::renderModel(mod, transform.GetTransform(), m_ShadowMapShader);
            }
        }
    }
    m_DirLightFB->bindDepthAttachment(6);
    m_DirLightFB->Unbind();
}

void SceneRenderer::render()
{
    shadowPass();
    m_FrameBuffer->Bind();
    // m_DirLightFB->bindDepthAttachment(6);

    {
        // Render Meshes
        auto view = m_Scene->m_Registry.view<TransformComponent, MeshRenderer>();
        for (auto e : view) {
            auto [transform, mesh] = view.get<TransformComponent, MeshRenderer>(e);
            Ref<Model> mod = AssetManager::getInstance().getAsset<Model>(mesh.model);
            if (!mod) {
                continue;
            }
            auto& overrides = mesh.materialOverrides;
            Renderer3D::renderModel(mod, transform.GetTransform(), overrides);
        }
    }
    m_FrameBuffer->Unbind();
}

void SceneRenderer::endScene()
{
    m_FrameBuffer->Bind();
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
    m_CameraUBO->setData(&m_CameraData, 0, sizeof(CameraData));
}
void SceneRenderer::setupLightData()
{
    m_DirLightData.clear();
    {
        auto view = m_Scene->m_Registry.view<TransformComponent, DirectionalLightComponent>();
        for (auto e : view) {
            auto [transform, light] = view.get<TransformComponent, DirectionalLightComponent>(e);
            i32 index = m_DirLightData.size();
            if (light.shadowTextureView->glID == 0) {
                light.shadowTextureView = CreateRef<TextureView>(m_DirLightFB->getDepthID(), ImageFormat::DEPTH32F, index);
            }

            float nearPlane = -200.0f, farPlane = 200.0f, size = 20.0f;
            glm::mat4 lightProjection = glm::ortho(-size, size, -size, size, nearPlane, farPlane);

            float dist = 100;
            glm::vec3 dir = glm::normalize(glm::quat(transform.Rotation) * glm::vec3(0, 0, -1));
            glm::vec3 pos = dir * dist;

            glm::mat4 dirLightView = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
            glm::mat4 viewProj = lightProjection * dirLightView;

            DirectionalLightData data = {
                .direction = glm::vec4(dir, 0.0f),
                .color = glm::vec4(light.color, light.intensity),
                .viewProj = viewProj,
            };
            m_DirLightData.push_back(data);
        }
    }

    m_PointLightData.clear();
    {
        auto view = m_Scene->m_Registry.view<TransformComponent, PointLightComponent>();
        for (auto e : view) {
            auto [transform, light] = view.get<TransformComponent, PointLightComponent>(e);
            LightData data = {
                glm::vec4(transform.Position, 0.0f),
                glm::vec4(transform.Rotation, 0.0f),
                glm::vec4(light.color * 255.0f, light.intensity),
                glm::vec4(0.0f, 0.0f, light.radius, 0.0f)
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
                glm::vec4(transform.Position, 0.0f),
                glm::vec4(spotlightDirection, 0.0f),
                glm::vec4(light.color * 255.0f, light.intensity),
                glm::vec4(glm::cos(glm::radians(light.cutOff)), glm::cos(glm::radians(light.outerCutOff)), light.radius, 1.0f)
            };
            m_PointLightData.push_back(data);
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

    m_PointLightUBO->setData(m_PointLightData.data(), 0, pLightDataSize);
    m_PointLightUBO->setData(&numPLights, maxPLightSize, sizeof(u32));

    u32 numDLights = m_DirLightData.size();
    int dLightDataSize = sizeof(DirectionalLightData) * numDLights;
    int maxDLightSize = sizeof(DirectionalLightData) * MAX_DIRECTIONAL_LIGHTS;
    m_DirLightUBO->setData(m_DirLightData.data(), 0, dLightDataSize);
    m_DirLightUBO->setData(&numDLights, maxDLightSize, sizeof(u32));
}
}
