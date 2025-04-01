#include "Rendering/OpenGL/OpenGL_Renderer.hpp"
#include <Rendering/Texture2DManager.hpp>

#include <imgui.h>

#include "glad/glad.h"

#include "GLFW/glfw3.h"
#include "Rendering/Renderer.hpp"
#include <GL/gl.h>

namespace Dimensional {

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
    }
    void Shutdown()
    {
    }

    void DrawIndexed(Ref<VAO> vao, Ref<Shader> shader)
    {
        shader->Bind();
        vao->Bind();
        glDrawElements(GL_TRIANGLES, vao->GetElementCount(), GL_UNSIGNED_INT, 0);
    }

}
}
