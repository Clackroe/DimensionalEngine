#ifndef DM_SCENERENDERER_HPP
#define DM_SCENERENDERER_HPP

#include "Log/log.hpp"
#include "Rendering/EnvironmentMap.hpp"
#include "Rendering/FrameBuffer.hpp"
#include "Rendering/RendererAPI.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include <Rendering/RenderGraph.hpp>
#include <Scene/Scene.hpp>
#include <buffer.hpp>
#include <memory>

namespace Dimensional {
#define MAX_POINTLIGHTS 256

#define MAX_DIRECTIONAL_LIGHTS 10
#define CASCADES 3

struct MeshData {
    Ref<Mesh> mesh;
    glm::mat4 transform;
    AssetHandle material;
};

struct alignas(16) CameraData {
    glm::mat4 viewProj;
    glm::vec3 uCameraPosition;
    float aspectRatio;
    glm::mat4 view;
    glm::mat4 proj;
    float fov;
    float near;
    float far;
};

struct alignas(16) DirectionalLightData {
    glm::vec4 direction;
    glm::vec4 color; // color, intensity
    glm::mat4 projections;
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
    ~SceneRenderer();
    SceneRenderer(Ref<Scene> scene)
        : m_Scene(scene)
    {

        tryInitStaticMembers();

        FrameBufferSettings fbs = {
            1920,
            1080,
            { RGBA32F, RGBA32F, RGBA32F, Depth },
            TEXTURE_2D
        };
        m_FrameBuffer = CreateRef<FrameBuffer>(fbs);

        RenderPass shadowPassData {};
        shadowPassData.debugName = "Scene Shadow Pass";
        shadowPassData.faceCulling = true;
        shadowPassData.culledFace = FaceCulling::FRONT;
        shadowPassData.depthTest = true;
        shadowPassData.depthFunction = DepthFunc::DEFAULT;
        shadowPassData.clearBuffer = ClearBuffer::DEPTH;
        shadowPassData.frameBuffer = s_DirLightFB;
        shadowPassData.renderShader = s_ShadowMapShader;
        shadowPassData.renderFunc = [this](Ref<Shader> shad, Ref<FrameBuffer> buf) {
            for (int index = 0; index < this->m_DirLightData.size(); index += 3) {
                s_ShadowMapShader->setInt("uDirLightIndex", index);
                for (auto& data : this->m_SceneMeshes) {
                    Renderer3D::renderMesh(data.mesh, s_ShadowMapShader, data.transform);
                }
            }
        };

        RenderPass mainPassData {};
        mainPassData.debugName = "Scene Main Pass";
        mainPassData.faceCulling = true;
        mainPassData.culledFace = FaceCulling::BACK;
        mainPassData.depthTest = true;
        mainPassData.depthFunction = DepthFunc::DEFAULT;
        mainPassData.clearBuffer = ClearBuffer::BOTH;
        mainPassData.frameBuffer = m_FrameBuffer;
        mainPassData.renderFunc = [this](Ref<Shader> shad, Ref<FrameBuffer> buf) {
            this->s_DirLightFB->bindDepthAttachment(6);

            for (auto& data : this->m_SceneMeshes) {
                Ref<Material> mat = AssetManager::getInstance().getAsset<Material>(data.material);
                Renderer3D::renderMesh(data.mesh, mat, data.transform);
            }
        };

        RenderPass skyBoxPassData {};
        skyBoxPassData.debugName = "Scene SkyBox Pass";
        skyBoxPassData.faceCulling = false;
        skyBoxPassData.depthTest = true;
        skyBoxPassData.depthFunction = DepthFunc::LEQUAL;
        skyBoxPassData.clearBuffer = ClearBuffer::NONE;
        skyBoxPassData.frameBuffer = m_FrameBuffer;
        skyBoxPassData.renderShader = s_CubeMapShader;
        skyBoxPassData.renderFunc = [this](Ref<Shader> shad, Ref<FrameBuffer> fb) {
            Renderer3D::renderCube(shad);
        };

        m_RenderGraph.pushRenderPass(shadowPassData);
        m_RenderGraph.pushRenderPass(mainPassData);
        m_RenderGraph.pushRenderPass(skyBoxPassData);
    }

    void beginScene(CameraData camData);
    void endScene();

    void setScene(Ref<Scene> scene) { m_Scene = scene; };

    void render();

    Ref<FrameBuffer> getFrameBuffer() { return m_FrameBuffer; }
    Ref<FrameBuffer> getFrameBufferTEMP() { return s_DirLightFB; };

private:
    void setupLightData();
    void setupCameraData();

    void gatherMeshData();

    void tryInitStaticMembers();

private:
    Ref<Scene> m_Scene;
    Ref<FrameBuffer> m_FrameBuffer;

    std::vector<LightData> m_PointLightData;
    CameraData m_CameraData;

    EnvironmentData m_CurrentEnvironmentMap;

    std::vector<DirectionalLightData> m_DirLightData;
    u32 m_DirLightDepthID;

    std::vector<MeshData> m_SceneMeshes;

    RenderGraph m_RenderGraph;

    static Ref<UniformBuffer> s_PointLightUBO;
    static Ref<UniformBuffer> s_CameraUBO;
    static Ref<UniformBuffer> s_DirLightUBO;
    static Ref<FrameBuffer> s_DirLightFB;
    static Ref<Shader> s_ShadowMapShader;
    static Ref<Shader> s_CubeMapShader;
};

}

#endif
