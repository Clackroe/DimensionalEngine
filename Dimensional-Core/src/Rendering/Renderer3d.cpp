#include "Log/log.hpp"
#include "Rendering/ElementBuffer.hpp"
#include <Rendering/Renderer3d.hpp>
#include <Rendering/RendererAPI.hpp>

namespace Dimensional {

static Ref<VertexArray> s_CubeVAO;
static Ref<ElementBuffer> s_CubeEB;

Renderer3DData Renderer3D::m_Data;

static void handleSubmit(Ref<Material>& mat, Mesh& mesh, Renderer3DData& data, RenderInstanceData iData)
{
    auto& batch = data.sectors[data.currentSector].materialBatches[mat->getShader()->ID][mat->handle][mesh.getVAO()->getGLID()];
    if (!batch.material) {
        batch.material = mat;
    }
    if (!batch.mesh) {
        batch.mesh = std::make_shared<Mesh>(mesh);
    }
    batch.instances.push_back(iData);
}
static void handleSubmit(Ref<Shader>& shad, Mesh& mesh, Renderer3DData& data, RenderInstanceData iData)
{
    auto& batch = data.sectors[data.currentSector].shaderOnlyBatches[shad->ID][mesh.getVAO()->getGLID()];
    if (!batch.mesh) {
        batch.mesh = std::make_shared<Mesh>(mesh);
    }
    batch.instances.push_back(iData);
}

static void generateCube()
{
    float vertices[] = {
        // Positions            // Normals          // Texture Coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        //
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };
    u32 indices[] = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    s_CubeVAO = CreateRef<VertexArray>();
    VertexBuffer vb = VertexBuffer(vertices, sizeof(vertices));
    s_CubeEB = CreateRef<ElementBuffer>(indices, sizeof(indices) / sizeof(u32));
    VertexLayout lb;
    lb.Push<float>(3);
    lb.Push<float>(3);
    lb.Push<float>(2);
    s_CubeVAO->AddBuffer(vb, lb);
}

void Renderer3D::Init()
{
    generateCube();
}

void Renderer3D::submitMesh(Mesh mesh, Ref<Material> material, glm::mat4 transform)
{
    handleSubmit(material, mesh, m_Data, { transform });
}

void Renderer3D::submitMesh(Mesh mesh, Ref<Shader> shader, glm::mat4 transform)
{
    handleSubmit(shader, mesh, m_Data, { transform });
}

void Renderer3D::submitModel(Ref<Model> model, glm::mat4 transform, Ref<Shader> shader)
{
    auto& meshes = model->getMeshes();
    for (u32 i = 0; i < meshes.size(); i++) {
        handleSubmit(shader, meshes[i], m_Data, { transform });
    }
}

void Renderer3D::submitModel(Ref<Model> model, glm::mat4 transform, std::vector<AssetHandle> matOverrirdes)
{
    auto& meshes = model->getMeshes();
    for (u32 i = 0; i < meshes.size(); i++) {
        Ref<Material> mat = AssetManager::getInstance().getAsset<Material>(model->getMaterials()[i]);
        if (!mat) {
            DM_CORE_WARN("NO MAT")
        }
        if (matOverrirdes[i]) {
            mat = AssetManager::getInstance().getAsset<Material>(matOverrirdes[i]);
        }
        handleSubmit(mat, meshes[i], m_Data, { transform });
    }
}

void Renderer3D::submitCube(Ref<Shader> shader, glm::mat4 transform)
{
    auto& batch = m_Data.sectors[m_Data.currentSector].shaderOnlyBatches[shader->ID][s_CubeVAO->getGLID()];
    batch.instances.push_back({ transform });
}

void Renderer3D::beginSector(std::function<void()> setupFunc)
{
    m_Data.sectors[m_Data.currentSector].setupFunc = setupFunc;
}

void Renderer3D::endSector(std::function<void()> cleanupFunc)
{
    m_Data.sectors[m_Data.currentSector].cleanupFunc = cleanupFunc;
    m_Data.currentSector++;
}

void Renderer3D::submitFrame()
{
    for (u32 i = 0; i < m_Data.sectors.size(); i++) {
        FrameSector sector = m_Data.sectors[i];

        if (sector.setupFunc) {
            sector.setupFunc();
        } else {
            DM_CORE_WARN("No SetupFunc Provided For Frame Sector");
        }
        //====
        for (auto& [shaderID, materialMap] : sector.materialBatches) {
            bool shaderBound = false;
            for (auto& [materialID, meshMap] : materialMap) {
                for (auto& [meshID, renderBatch] : meshMap) {
                    renderBatch.material->bind(!shaderBound);
                    for (auto& i : renderBatch.instances) {
                        renderBatch.material->getShader()->setMat4("model", i.transform);
                        RendererAPI::getInstance().renderIndexed(*renderBatch.mesh->vao, *renderBatch.mesh->eb, renderBatch.material->getShader());
                    }
                }
            }
        }

        // Process shader-only batches (shader -> mesh -> instances)
        for (auto& [shaderID, meshMap] : sector.shaderOnlyBatches) {
            bool shaderBound = false;
            for (auto& [meshID, renderBatch] : meshMap) {
                renderBatch.material->bind(!shaderBound);
                for (auto& i : renderBatch.instances) {
                    renderBatch.material->getShader()->setMat4("model", i.transform);
                    RendererAPI::getInstance().renderIndexed(*renderBatch.mesh->vao, *renderBatch.mesh->eb, renderBatch.material->getShader());
                }
            }
        }
        //====

        if (sector.cleanupFunc) {
            sector.cleanupFunc();
        } else {
            DM_CORE_WARN("No CleanupFunc Provided For Frame Sector");
        }
    }
    m_Data.sectors.clear();
};

}
