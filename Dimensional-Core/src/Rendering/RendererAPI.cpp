#include "Rendering/Renderer3d.hpp"
#include <Rendering/RendererAPI.hpp>
#include <glad.h>

namespace Dimensional {

void RendererAPI::Init(GLADloadproc proc)
{

    DM_CORE_ASSERT(gladLoadGLLoader(proc), "Failed to initialize GLAD. Aborting");
    glEnable(GL_DEPTH_TEST);
    Renderer3D::Init();
    DM_CORE_INFO("RendererAPI initialized");
}

void RendererAPI::beginFrame() { }
void RendererAPI::endFrame() { }

void RendererAPI::setClearColor(const glm::vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}
void RendererAPI::clearBuffer(bool depthOnly)
{
    if (depthOnly) {
        glClear(GL_DEPTH_BUFFER_BIT);
        return;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererAPI::renderArrays(VertexArray vao, u32 triangleCount, Ref<Shader>& shader)
{
    shader->use();
    vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, triangleCount);
}
void RendererAPI::renderIndexed(VertexArray& vao, ElementBuffer& eb, const Ref<Shader>& shader)
{
    shader->use();
    vao.Bind();
    eb.Bind();

    glDrawElements(GL_TRIANGLES, eb.getCount(), GL_UNSIGNED_INT, nullptr);
}
void RendererAPI::setDepthFunc(DepthFunc f)
{
    switch (f) {
    case DepthFunc::NEVER:
        glDepthFunc(GL_NEVER);
        break;
    case DepthFunc::LESS:
        glDepthFunc(GL_LESS);
        break;
    case DepthFunc::EQUAL:
        glDepthFunc(GL_EQUAL);
        break;
    case DepthFunc::LEQUAL:
        glDepthFunc(GL_LEQUAL);
        break;
    case DepthFunc::GREATER:
        glDepthFunc(GL_GREATER);
        break;
    case DepthFunc::NOTEQUAL:
        glDepthFunc(GL_NOTEQUAL);
        break;
    case DepthFunc::GEQUAL:
        glDepthFunc(GL_GEQUAL);
        break;
    case DepthFunc::ALWAYS:
        glDepthFunc(GL_ALWAYS);
        break;
    }
}

void RendererAPI::setDepthTest(bool enable)
{
    if (enable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}
void RendererAPI::setBlending(bool enable)
{
    if (enable) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
}

}
