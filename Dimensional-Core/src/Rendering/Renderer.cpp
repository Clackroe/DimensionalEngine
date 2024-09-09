#include "Core/Assets/AssetManager.hpp"
#include "Rendering/VertexBuffer.hpp"
#include "core.hpp"
#include "glm/matrix.hpp"
#include <Core/Application.hpp>
#include <Rendering/Mesh.hpp>
#include <Rendering/Model.hpp>
#include <Rendering/Renderer.hpp>
#include <Rendering/Shader.hpp>
#include <Rendering/Texture.hpp>
#include <glad/glad.h>

namespace Dimensional {

Renderer* Renderer::s_RendererRef = nullptr;

void Renderer::Init()
{
    if (s_RendererRef) {
        DM_CORE_WARN("Renderer Already initialized!");
        return;
    }

    generatePrimitives();
    AssetManager::loadMaterial();
    s_RendererRef = this;
    DM_CORE_INFO("Renderer Initialized.")
    m_FrameBuffer = CreateRef<FrameBuffer>(fbs);
    // TODO: IDEK but I hate this
    m_PBRShader = AssetManager::loadShader(Application::getApp().engineAssetDirectory + "/Shaders/PBRWithLighting.glsl");
};

void Renderer::submitLight(LightData data)
{
    Renderer& ref = m_GetRenderer();
    ref.m_LightData.push_back(data);
}

void Renderer::renderCube(Ref<Shader>& shader)
{
    Renderer& ref = m_GetRenderer();
    Renderer::renderVAO(*ref.cubeVao, *ref.cubeEb, shader);
}
void Renderer::renderSphere(Ref<Shader>& shader)
{
    Renderer& ref = m_GetRenderer();
    shader->use();
    ref.sphereVao->Bind();
    ref.sphereEb->Bind();

    glDrawElements(GL_TRIANGLE_STRIP, ref.sphereEb->getCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::renderCube(Ref<Material>& mat, glm::mat4 transform)
{
    Renderer& ref = m_GetRenderer();
    mat->bind(ref.m_PBRShader);
    ref.m_PBRShader->setMat4("model", transform);
    Renderer::renderCube(ref.m_PBRShader);
}
void Renderer::renderMesh(Mesh& mesh, Ref<Material>& mat, glm::mat4 transform)
{

    Renderer& ref = m_GetRenderer();
    mat->bind(ref.m_PBRShader);
    ref.setupLightData();
    ref.m_PBRShader->setMat4("model", transform);

    Renderer::renderVAO(*mesh.vao, *mesh.eb, ref.m_PBRShader);
}

void Renderer::renderModel(Model& model, Ref<Material>& mat, glm::mat4 transform)
{
    for (u32 i = 0; i < model.m_Meshes.size(); i++) {
        Mesh& mesh = model.m_Meshes[i];
        Renderer::renderMesh(model.m_Meshes[i], mat, transform);
    }
}

void Renderer::generatePrimitives()
{
    generateSphere();
    generateCube();
    DM_CORE_INFO("Primitives Initialized");
};

void Renderer::renderVAO(VertexArray vao, u32 triangleCount, Ref<Shader>& shader)
{
    shader->use();
    vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, triangleCount);
}
void Renderer::renderVAO(const VertexArray& vao, const ElementBuffer& eb, const Ref<Shader>& shader)
{
    shader->use();
    vao.Bind();
    eb.Bind();

    glDrawElements(GL_TRIANGLES, eb.getCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::setupCameraData()
{
    m_PBRShader->use();
    m_PBRShader->setMat4("viewProj", m_CameraData.viewProj);
    m_PBRShader->setVec3("uCameraPosition", m_CameraData.uCameraPosition.x, m_CameraData.uCameraPosition.y, m_CameraData.uCameraPosition.z);
}

void Renderer::setupLightData()
{
    Renderer& ref = m_GetRenderer();

    ref.m_PBRShader->use();

    u32 numPointLights = 0;
    u32 numSpotLights = 0;
    /*
     *     vec3 position;
    vec3 direction;
    vec3 color;

    float intensity;
    float cutOff; // Spotlight cutoff angle
    float outerCutOff; // Spotlight outer cutoff angle

    // Attenuation parameters
    float constant;
    float linear;
    float quadratic;

     * */

    for (u32 i = 0; i < ref.m_LightData.size(); ++i) {
        LightData& light = ref.m_LightData[i];

        std::string pre;
        if (light.cutOff == 0.0f && light.outerCutOff == 0.0f) {
            pre = "uPointLights[" + std::to_string(numPointLights) + "]";
            numPointLights++;
        } else {
            pre = "uSpotLights[" + std::to_string(numSpotLights) + "]";
            numSpotLights++;
        }
        ref.m_PBRShader->setVec3(pre + ".position", light.position.x, light.position.y, light.position.z);
        ref.m_PBRShader->setVec3(pre + ".direction", light.direction.x, light.direction.y, light.direction.z);
        ref.m_PBRShader->setVec3(pre + ".color", light.color.x, light.color.y, light.color.z);
        ref.m_PBRShader->setFloat(pre + ".cutOff", light.cutOff);
        ref.m_PBRShader->setFloat(pre + ".outerCutOff", light.outerCutOff);
        ref.m_PBRShader->setFloat(pre + ".constant", light.constant);
        ref.m_PBRShader->setFloat(pre + ".constant", light.constant);
        ref.m_PBRShader->setFloat(pre + ".linear", light.quadratic);
    }
    ref.m_PBRShader->setInt("uNumPointLights", numPointLights);
    ref.m_PBRShader->setInt("uNumSpotLights", numSpotLights);
}

void Renderer::beginScene(CameraData data)
{
    Renderer& ref = m_GetRenderer();
    ref.m_FrameBuffer->Bind();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ref.m_CameraData = data;
    ref.setupCameraData();
    ref.setupLightData();
}
void Renderer::endScene()
{
    Renderer& ref = m_GetRenderer();
    ref.m_FrameBuffer->Unbind();
    // Flush Data
    ref.m_LightData.erase(ref.m_LightData.begin(), ref.m_LightData.end());
}

void Renderer::generateCube()
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

    cubeVao = CreateRef<VertexArray>();
    VertexBuffer vb = VertexBuffer(vertices, sizeof(vertices));
    cubeEb = CreateRef<ElementBuffer>(indices, sizeof(indices) / sizeof(u32));
    VertexLayout lb;
    lb.Push<float>(3);
    lb.Push<float>(3);
    lb.Push<float>(2);
    cubeVao->AddBuffer(vb, lb);
}

void Renderer::generateSphere()
{
    u32 indexCount = 0;

    sphereVao = CreateRef<VertexArray>();
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uv;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        } else {
            for (int x = X_SEGMENTS; x >= 0; --x) {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    indexCount = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i) {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0) {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
        if (uv.size() > 0) {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
    }
    VertexBuffer vb = VertexBuffer(data.data(), data.size() * sizeof(float));
    sphereEb = CreateRef<ElementBuffer>(indices.data(), indexCount);
    VertexLayout lb;
    lb.Push<float>(3);
    lb.Push<float>(3);
    lb.Push<float>(2);

    sphereVao->AddBuffer(vb, lb);
};

}
