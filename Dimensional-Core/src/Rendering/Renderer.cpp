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

Ref<Shader> Renderer::createShader(std::string vertexShader, std::string fragShader)
{
    Renderer& ref = m_GetRenderer();

    // Hash t = std::hash<std::string> {}(vertexShader + fragShader);
    // auto it = ref.m_ShaderMap.find(t);
    // if (it != ref.m_ShaderMap.end()) {
    //     return it->second->hash;
    // } else {
    Ref<Shader> shader = CreateRef<Shader>(vertexShader, fragShader);

    // If necessary, ensure there is no hash collisions

    ref.m_ShaderMap[shader->name] = shader;
    return shader;
    // }
}
Ref<Texture> Renderer::createTexture(std::string path, bool retainInMemory)
{
    Renderer& ref = m_GetRenderer();

    // Hash t = std::hash<std::string> {}(path);
    // auto it = ref.m_TextureMap.find(t);
    // if (it != ref.m_TextureMap.end()) {
    //     return it->second->hash;
    // } else {

    Ref<Texture> tex = CreateRef<Texture>(path, retainInMemory);

    // If necessary, ensure there is no hash collisions

    ref.m_TextureMap[tex->name] = tex;
    return tex;
    // }
}
}
