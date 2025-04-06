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

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

// Global persistant buffers, to support multidraw indirect,
// and reduce draw-calls significantly

#define INITIAL_MAX_VERTEX_COUNT 1024

Ref<GPUBuffer> g_VertexBuffer;
size_t tempVertexHead = 0;

Ref<GPUBuffer> g_IndexBuffer;
size_t tempIndexHead = 0;

Ref<GPUBuffer> g_CommandBuffer;

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
            data.usage = GPUBufferUsage::DYNAMIC;
            data.persistant = true;
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
            data.slot = 0;
            data.usage = GPUBufferUsage::DYNAMIC;
            data.persistant = true;
            data.type = GPUBufferType::ELEMENT;
            data.sizeBytes = INITIAL_MAX_VERTEX_COUNT * sizeof(u32);
            data.data = nullptr;

            g_IndexBuffer = GPUBuffer::Create(data);
        }

        {
            GPUBufferData data;
            data.slot = 0;
            data.usage = GPUBufferUsage::DYNAMIC;
            data.persistant = true;
            data.type = GPUBufferType::COMMAND;
            data.sizeBytes = INITIAL_MAX_VERTEX_COUNT * sizeof(u32);
            data.data = nullptr;

            g_CommandBuffer = GPUBuffer::Create(data);

            std::vector<DrawElementsIndirectCommand> cm;
            DrawElementsIndirectCommand cmd;
            cmd.count = 6;
            cmd.baseVertex = 0;
            cmd.firstIndex = 0;
            cmd.baseInstance = 0;
            cmd.instanceCount = 1;
            cm.push_back(cmd);

            g_CommandBuffer->SetData(cm.data(), 0, cm.size() * sizeof(DrawElementsIndirectCommand));
        }
    }
    void Shutdown()
    {
        g_IndexBuffer = nullptr;
        g_VertexBuffer = nullptr;
        g_CommandBuffer = nullptr;
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
        vao->Bind();
        // shader->Bind();
        g_IndexBuffer->Bind(0);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, 1, 0);

        // glCheckError();
        // glDrawElements(GL_TRIANGLES, vao->GetElementCount(), GL_UNSIGNED_INT, 0);
    }
}
}
