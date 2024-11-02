#include "Rendering/RendererAPI.hpp"
#include "Scene/Components.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include <Core/Camera.hpp>
#include <Rendering/Renderer3d.hpp>
#include <Rendering/SceneRenderer.hpp>
namespace Dimensional {

static std::vector<glm::mat4> getCascadedLightMatricies(glm::mat4 cameraView, glm::vec3 lightDir, float aspect, float fov, int cascades, float camNear, float camFar, float zMult = 10.0)
{
    std::vector<glm::mat4> out;

    const float cascadeRatio = (1.0f / cascades) * camFar;

    for (int i = 0; i < cascades; i++) {
        float near = camNear + (i * cascadeRatio);
        float far = cascadeRatio + (i * cascadeRatio);
        glm::mat4 viewProj = Camera::calcTightOrthoProjection(cameraView, lightDir, near, far, fov, aspect, zMult);
        out.push_back(viewProj);
    }
    return out;
}

// ==

// ============== END SHADOW UTILS =============

Ref<UniformBuffer> SceneRenderer::m_PointLightUBO;
Ref<UniformBuffer> SceneRenderer::m_CameraUBO;

Ref<UniformBuffer> SceneRenderer::m_DirLightUBO;
Ref<FrameBuffer> SceneRenderer::m_DirLightFB;
Ref<Shader> SceneRenderer::m_ShadowMapShader;

SceneRenderer::~SceneRenderer()
{
    auto view = m_Scene->m_Registry.view<TransformComponent, DirectionalLightComponent>();
    for (auto e : view) {
        auto [transform, light] = view.get<TransformComponent, DirectionalLightComponent>(e);
        light.shadowTextureView = CreateRef<TextureView>();
    }
}

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
    RendererAPI::getInstance().setCulling(FaceCulling::FRONT);
    for (int index = 0; index < m_DirLightData.size(); index += 3) {

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
    RendererAPI::getInstance().setCulling(FaceCulling::DEFAULT);
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
    m_CubeMapShader->setInt("environmentMap", 8);
    if (m_CurrentEnvironmentMap.envMap) {
        m_CurrentEnvironmentMap.envMap->bind();
        m_CubeMapShader->setFloat("uLod", m_CurrentEnvironmentMap.lod);
    }

    RendererAPI::getInstance().setDepthFunc(DepthFunc::LEQUAL);
    RendererAPI::getInstance().enableCulling(false);
    Renderer3D::renderCube(m_CubeMapShader);
    RendererAPI::getInstance().enableCulling(true);
    RendererAPI::getInstance().setDepthFunc(DepthFunc::DEFAULT);

    m_FrameBuffer->Unbind();
}

void SceneRenderer::setupCameraData()
{
    m_CameraUBO->setData(&m_CameraData, 0, sizeof(CameraData));
}
void SceneRenderer::setupLightData()
{
    m_DirLightData.clear();
    m_DirLightUBO->zeroOut();
    {
        auto view = m_Scene->m_Registry.view<TransformComponent, DirectionalLightComponent>();
        for (auto e : view) {
            auto [transform, light] = view.get<TransformComponent, DirectionalLightComponent>(e);
            i32 index = m_DirLightData.size();

            if (light.shadowTextureView->glID == 0) {
                light.shadowTextureView = CreateRef<TextureView>(m_DirLightFB->getDepthID(), ImageFormat::DEPTH32F, index);
            }

            glm::vec3 dir = glm::normalize(glm::quat(transform.Rotation) * glm::vec3(0, 0, -1));

            std::vector<glm::mat4> cascades = getCascadedLightMatricies(m_CameraData.view, dir, m_CameraData.aspectRatio, m_CameraData.fov, CASCADES, m_CameraData.near, m_CameraData.far, 10.0);
            for (glm::mat4& vp : cascades) {
                DirectionalLightData data = {
                    .direction = glm::vec4(dir, 0.0f),
                    .color = glm::vec4(light.color, light.intensity),
                    .projections = vp,
                };
                m_DirLightData.push_back(data);
            }
        }
    }

    m_PointLightData.clear();
    m_PointLightUBO->zeroOut();
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
    int maxDLightSize = sizeof(DirectionalLightData) * MAX_DIRECTIONAL_LIGHTS * CASCADES;
    m_DirLightUBO->setData(m_DirLightData.data(), 0, dLightDataSize);
    m_DirLightUBO->setData(&numDLights, maxDLightSize, sizeof(u32));
}
}
