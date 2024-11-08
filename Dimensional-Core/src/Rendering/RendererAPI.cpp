#include "Rendering/Renderer3d.hpp"
#include <Rendering/RendererAPI.hpp>
#include <glad.h>

namespace Dimensional {

void RendererAPI::Init(GLADloadproc proc)
{

    DM_CORE_ASSERT(gladLoadGLLoader(proc), "Failed to initialize GLAD. Aborting");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    Renderer3D::Init();
    GLint max_layers;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers);
    DM_CORE_WARN("MAX ARRAY TEXTURE LAYERS: {}", max_layers);

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

void RendererAPI::enableDepthTest(bool enable)
{
    if (enable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}
void RendererAPI::enableBlending(bool enable)
{
    if (enable) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
}
void RendererAPI::enableCulling(bool enable)
{
    if (enable) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

void RendererAPI::setCulling(FaceCulling cull)
{
    switch (cull) {
    case FaceCulling::FRONTANDBACK:
        glCullFace(GL_FRONT_AND_BACK);
        break;
    case FaceCulling::FRONT:
        glCullFace(GL_FRONT);
        break;
    case FaceCulling::BACK:
        glCullFace(GL_BACK);
        break;
    }
}

}
