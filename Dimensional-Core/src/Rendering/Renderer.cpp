#include "Rendering/VertexBuffer.hpp"
#include "core.hpp"
#include <Rendering/Mesh.hpp>
#include <Rendering/Model.hpp>
#include <Rendering/Renderer.hpp>
#include <Rendering/Shader.hpp>
#include <Rendering/Texture.hpp>
#include <glad/glad.h>

namespace Dimensional {

Renderer* Renderer::s_RendererRef = nullptr;

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
void Renderer::renderMesh(Mesh& mesh, Ref<Shader>& shader)
{
    // DM_CORE_INFO("MeshRender, Textures: {0}", mesh.textures.size())
    for (u32 i = 0; i < mesh.textures.size(); i++) {
        Ref<Texture> t = Renderer::getTexture(mesh.textures[i].name);
        t->bind(i);
        std::string type = mesh.textures[i].type;
        if (type == "texture_albedo") {
            shader->setInt("albedoMap", i);
        } else if (type == "texture_normal") {
            shader->setInt("normalMap", i);
        } else if (type == "texture_metallic") {
            shader->setInt("metallicMap", i);
        } else if (type == "texture_roughness") {
            shader->setInt("roughnessMap", i);
        } else if (type == "texture_ao") {
            shader->setInt("aoMap", i);
        } else {
            DM_CORE_WARN("Unknown Texture Type ({0}): {1}", mesh.textures[i].type, mesh.textures[i].name);
        }
    }
    Renderer::renderVAO(*mesh.vao, *mesh.eb, shader);
}

void Renderer::renderModel(Model& model, Ref<Shader>& shader)
{
    for (u32 i = 0; i < model.m_Meshes.size(); i++) {
        Mesh& mesh = model.m_Meshes[i];
        Renderer::renderMesh(model.m_Meshes[i], shader);
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

void Renderer::beginScene()
{
    Renderer& ref = m_GetRenderer();
    ref.m_FrameBuffer->Bind();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Renderer::endScene()
{
    Renderer& ref = m_GetRenderer();
    ref.m_FrameBuffer->Unbind();
}

Ref<Shader> Renderer::createShader(std::string path)
{
    Renderer& ref = m_GetRenderer();

    Ref<Shader> shader = CreateRef<Shader>(path);

    ref.m_ShaderMap[shader->name] = shader;
    return shader;
}

Ref<Shader> Renderer::createShader(std::string vertexShader, std::string fragShader)
{
    Renderer& ref = m_GetRenderer();

    Ref<Shader> shader = CreateRef<Shader>(vertexShader, fragShader);

    ref.m_ShaderMap[shader->name] = shader;
    return shader;
}

Ref<Texture> Renderer::createTexture(std::string path, bool retainInMemory)
{
    Renderer& ref = m_GetRenderer();

    Ref<Texture> tex = CreateRef<Texture>(path, retainInMemory);

    ref.m_TextureMap[tex->name] = tex;
    return tex;
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
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
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
