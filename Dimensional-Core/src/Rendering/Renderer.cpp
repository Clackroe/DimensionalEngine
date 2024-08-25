#include "core.hpp"
#include <Rendering/Renderer.hpp>
#include <Rendering/Shader.hpp>
#include <Rendering/Texture.hpp>
#include <glad/glad.h>

namespace Dimensional {

Renderer* Renderer::s_RendererRef = nullptr;

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
}
