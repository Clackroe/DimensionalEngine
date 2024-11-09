#include "Asset/AssetManager.hpp"
#include "Log/log.hpp"
#include "Rendering/RendererAPI.hpp"
#include "Scene/Components.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include <Core/Camera.hpp>
#include <Rendering/Renderer3d.hpp>
#include <Rendering/SceneRenderer.hpp>
namespace Dimensional {

// https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-10-parallel-split-shadow-maps-programmable-gpus
static std::vector<float> splitCascadesUniform(int cascades, float near, float far)
{
    std::vector<float> out;

    for (int i = 1; i < cascades; i++) {
        out.push_back(near + ((far - near) * (float(i) / cascades)));
    }
    out.push_back(far);

    return out;
}

static std::vector<float> splitCascadesLog(int cascades, float near, float far)
{
    std::vector<float> out;
    for (int i = 0; i < cascades; i++) {
        float ratio = static_cast<float>(i + 1) / cascades; // i+1 because we are not starting at the near plane
        out.push_back(near * glm::pow(far / near, ratio));
    }
    return out;
}

static std::vector<float> splitCascadesPracticalSplitSum(int cascades, float near, float far, float lamda = 0.7)
{
    std::vector<float> out;

    std::vector<float> uniformValues = splitCascadesUniform(cascades, near, far);
    std::vector<float> logValues = splitCascadesLog(cascades, near, far);
    for (int i = 0; i < cascades; i++) {
        float logPart = lamda * logValues[i];
        float uniPart = (1.0f - lamda) * uniformValues[i];
        out.push_back(logPart + uniPart);
    }
    return out;
}

static std::vector<glm::mat4> getCascadedLightMatricies(glm::mat4 cameraView, glm::vec3 lightDir, float aspect, float fov, std::vector<float> splits, float camNear, float camFar, float zMult = 10.0)
{
    std::vector<glm::mat4> out;

    float prevFar = camNear;
    for (int i = 0; i < splits.size(); i++) {
        float near = prevFar;
        float far = splits[i];
        prevFar = far;

        glm::mat4 viewProj = Camera::calcTightOrthoProjection(cameraView, lightDir, near, far, fov, aspect, zMult);
        out.push_back(viewProj);
    }
    return out;
}

// ==

// ============== END SHADOW UTILS =============

Ref<UniformBuffer> SceneRenderer::s_PointLightUBO;
Ref<UniformBuffer> SceneRenderer::s_CameraUBO;

Ref<UniformBuffer> SceneRenderer::s_DirLightUBO;
Ref<FrameBuffer> SceneRenderer::s_DirLightFB;
Ref<Shader> SceneRenderer::s_ShadowMapShader;
Ref<Shader> SceneRenderer::s_CubeMapShader;

SceneRenderer::~SceneRenderer()
{
    auto view = m_Scene->m_Registry.view<TransformComponent, DirectionalLightComponent>();
    for (auto e : view) {
        auto [transform, light] = view.get<TransformComponent, DirectionalLightComponent>(e);
        light.shadowTextureView = CreateRef<TextureView>();
    }
}

void SceneRenderer::tryInitStaticMembers()
{
    if (!s_DirLightFB) {
        FrameBufferSettings dfbs = {
            2048,
            2048,
            { Shadow },
            ARRAY_2D,
            MAX_DIRECTIONAL_LIGHTS * CASCADES
        };
        s_DirLightFB = CreateRef<FrameBuffer>(dfbs);
    }

    if (!s_CameraUBO) {
        s_CameraUBO = CreateRef<UniformBuffer>(sizeof(CameraData), 0);
    }
    if (!s_PointLightUBO) {
        s_PointLightUBO = CreateRef<UniformBuffer>(MAX_POINTLIGHTS * sizeof(LightData) + sizeof(u32), 1);
    }

    if (!s_ShadowMapShader) {
        s_ShadowMapShader = CreateRef<Shader>("Assets/Shaders/ShadowMap.glsl");
    }

    if (!s_DirLightUBO) {
        s_DirLightUBO = CreateRef<UniformBuffer>(MAX_DIRECTIONAL_LIGHTS * CASCADES * sizeof(DirectionalLightData) + sizeof(u32) + 12 + (16 * CASCADES), 2);
    }

    if (!s_CubeMapShader) {
        s_CubeMapShader = CreateRef<Shader>("Assets/Shaders/CubeMap.glsl");
    }
}

void SceneRenderer::beginScene(CameraData camData)
{
    m_CameraData = camData;
    setupLightData();
    setupCameraData();
    gatherMeshData();
}

void SceneRenderer::gatherMeshData()
{
    auto view = m_Scene->m_Registry.view<TransformComponent, MeshRenderer>();
    for (auto e : view) {
        auto [transform, model] = view.get<TransformComponent, MeshRenderer>(e);
        Ref<Model> mod = AssetManager::getInstance().getAsset<Model>(model.model);
        if (!mod) {
            continue;
        }
        auto& overrides = model.materialOverrides;
        std::vector<Ref<Mesh>> meshes = mod->getMeshes();
        u32 index = 0;

        for (auto& mesh : meshes) {

            AssetHandle material;
            if (overrides[index]) {
                material = overrides[index];
            } else {
                material = mod->getMaterials()[index];
            }
            MeshData data {
                .mesh = mesh,
                .transform = transform.GetTransform(),
                .material = material
            };
            m_SceneMeshes.push_back(std::move(data));
            index++;
        }
    }
};

void SceneRenderer::render()
{
    m_RenderGraph.execute();
}

void SceneRenderer::endScene()
{
    // m_FrameBuffer->Bind();
    // s_CubeMapShader->setInt("environmentMap", 8);
    // if (m_CurrentEnvironmentMap.envMap) {
    //     m_CurrentEnvironmentMap.envMap->bind();
    //     s_CubeMapShader->setFloat("uLod", m_CurrentEnvironmentMap.lod);
    // }
    //
    // RendererAPI::getInstance().setDepthFunc(DepthFunc::LEQUAL);
    // RendererAPI::getInstance().disableCulling();
    // RendererAPI::getInstance().enableCulling();
    // RendererAPI::getInstance().setDepthFunc(DepthFunc::DEFAULT);
    //
    // m_FrameBuffer->Unbind();

    m_SceneMeshes.clear();
}

void SceneRenderer::setupCameraData()
{
    s_CameraUBO->setData(&m_CameraData, 0, sizeof(CameraData));
}

void SceneRenderer::setupLightData()
{
    std::vector<float> cascadeSplits = splitCascadesPracticalSplitSum(CASCADES, m_CameraData.near, m_CameraData.far);
    std::vector<float> paddedSplits;
    paddedSplits.resize(cascadeSplits.size() * 4);
    for (int i = 0; i < cascadeSplits.size(); i++) {
        paddedSplits[i * 4] = cascadeSplits[i];
    }

    m_DirLightData.clear();
    s_DirLightUBO->zeroOut();
    {
        auto view = m_Scene->m_Registry.view<TransformComponent, DirectionalLightComponent>();
        for (auto e : view) {
            auto [transform, light] = view.get<TransformComponent, DirectionalLightComponent>(e);
            i32 index = m_DirLightData.size();

            if (light.shadowTextureView->glID == 0) {
                light.shadowTextureView = CreateRef<TextureView>(s_DirLightFB->getDepthID(), ImageFormat::DEPTH32F, index);
            }

            glm::vec3 dir = glm::normalize(glm::quat(transform.Rotation) * glm::vec3(0, 0, -1));

            std::vector<glm::mat4> cascades = getCascadedLightMatricies(m_CameraData.view, dir, m_CameraData.aspectRatio, m_CameraData.fov, cascadeSplits, m_CameraData.near, m_CameraData.far, 10.0);
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
    s_PointLightUBO->zeroOut();
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

    s_PointLightUBO->setData(m_PointLightData.data(), 0, pLightDataSize);
    s_PointLightUBO->setData(&numPLights, maxPLightSize, sizeof(u32));

    u32 numDLights = m_DirLightData.size();
    int dLightDataSize = sizeof(DirectionalLightData) * numDLights;
    int maxDLightSize = sizeof(DirectionalLightData) * MAX_DIRECTIONAL_LIGHTS * CASCADES;
    s_DirLightUBO->setData(m_DirLightData.data(), 0, dLightDataSize);
    s_DirLightUBO->setData(&numDLights, maxDLightSize, sizeof(u32));
    s_DirLightUBO->setData(paddedSplits.data(), maxDLightSize + sizeof(u32) + 12 /*Alignment*/, 16 * CASCADES);
}
}
