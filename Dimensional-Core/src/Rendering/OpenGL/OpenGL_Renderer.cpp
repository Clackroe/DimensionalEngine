#include "Rendering/OpenGL/OpenGL_Renderer.hpp"
#include <Rendering/Texture2DManager.hpp>

#include <imgui.h>

#include "Rendering/GPUBuffer.hpp"
#include "Rendering/SubMesh.hpp"
#include "Rendering/SubMeshManager.hpp"
#include "glad/glad.h"

#include "GLFW/glfw3.h"
#include "Rendering/Renderer.hpp"
#include <GL/gl.h>

namespace Dimensional {

// Global persistant buffers, to support multidraw indirect,
// and reduce draw-calls significantly

#define INITIAL_MAX_VERTEX_COUNT 1024

Ref<GPUBuffer> g_VertexBuffer;
size_t tempVertexHead = 0;

Ref<GPUBuffer> g_IndexBuffer;
size_t tempIndexHead = 0;

namespace OpenGLRenderer {

    RendererAPI GetAPI()
    {
        return RendererAPI {
            &Init,
            &Shutdown,
            GraphicsAPI::OPENGL
        };
    }
    void SetClearColor(glm::vec3 color)
    {
        glClearColor(color.r, color.g, color.b, 1.0f);
    }

    void ClearScreen(ClearBuffer cb)
    {
        switch (cb) {
        case ClearBuffer::COLOR:
            glClear(GL_COLOR_BUFFER_BIT);
            return;
        case ClearBuffer::DEPTH:
            glClear(GL_DEPTH_BUFFER_BIT);
            return;
        case ClearBuffer::BOTH:
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            return;
        case ClearBuffer::NONE:
            return;
        }
    }

    void SetWindowHints()
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    void ImGuiDrawTexture(Ref<Texture2D> tex, u32 width, u32 height)
    {
        OpenGLTexture2D t = Texture2DManager::GetOpenGLTexture(tex->GetUUID());
        ImGui::Image(reinterpret_cast<ImTextureID>(t.m_GLID), ImVec2 { (float)width, (float)height }, { 0, 1 }, { 1, 0 });
    }

    void Init(Window& window)
    {
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        // Persistant Buffers

        {
            GPUBufferData data;
            data.slot = 8;
            data.usage = GPUBufferUsage::DYNAMIC_PERSIST;
            data.type = GPUBufferType::STORAGE;
            data.sizeBytes = INITIAL_MAX_VERTEX_COUNT * sizeof(Vertex);
            data.data = nullptr;

            g_VertexBuffer = GPUBuffer::Create(data);
            //
            // std::vector<Vertex> vs(30);
            // for (int i = 0; i < vs.size() - 5; i++) {
            //     Vertex v;
            //     v.Tangent = glm::vec3(2);
            //     v.Position = glm::vec3(10);
            //     v.Normal = glm::vec3(0.5);
            //     v.BiTangent = glm::vec3(0.2);
            //     v.TexCoords = glm::vec2(0.9);
            //     vs[i] = v;
            // }
            //
            // Vertex v;
            // v.Tangent = glm::vec3(420);
            // v.Position = glm::vec3(69);
            // v.Normal = glm::vec3(1);
            // v.BiTangent = glm::vec3(420);
            // v.TexCoords = glm::vec2(60);
            // vs[25] = v;
            //
            // g_VertexBuffer->SetData(vs.data(), 0, vs.size() * sizeof(Vertex));
        }

        {
            GPUBufferData data;
            data.slot = 4;
            data.usage = GPUBufferUsage::DYNAMIC_PERSIST;
            data.type = GPUBufferType::STORAGE;
            data.sizeBytes = INITIAL_MAX_VERTEX_COUNT * sizeof(u32);
            data.data = nullptr;

            g_IndexBuffer = GPUBuffer::Create(data);
        }
    }
    void Shutdown()
    {
        g_IndexBuffer = nullptr;
        g_VertexBuffer = nullptr;
    }

    SubMeshIndexData AddSubMesh(const SubMeshData& data)
    {
        SubMeshIndexData out;
        out.count = data.indices.size();
        out.firstIndex = tempIndexHead;
        out.baseVertex = tempVertexHead;
        out.baseInstance = 0;

        g_VertexBuffer->SetData(data.vertices.data(), tempVertexHead * sizeof(Vertex), data.vertices.size() * sizeof(Vertex));
        g_IndexBuffer->SetData(data.indices.data(), tempIndexHead * sizeof(u32), data.indices.size() * sizeof(u32));
        tempVertexHead += data.vertices.size();
        tempIndexHead += data.indices.size();

        return out;
    };

    void DrawIndexed(Ref<VAO> vao, Ref<Shader> shader)
    {
        shader->Bind();
        vao->Bind();
        glDrawElements(GL_TRIANGLES, vao->GetElementCount(), GL_UNSIGNED_INT, 0);
    }
}
}
