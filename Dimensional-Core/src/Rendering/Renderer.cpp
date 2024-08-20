#include "core.hpp"
#include <Core/Shader.hpp>
#include <Rendering/Renderer.hpp>

namespace Dimensional {

Renderer* Renderer::s_RendererRef = nullptr;

Hash Renderer::createShader(std::string vertexShader, std::string fragShader)
{
    Renderer& ref = m_GetRenderer();

    Hash t = std::hash<std::string> {}(vertexShader + fragShader);
    auto it = ref.m_ShaderMap.find(t);
    if (it != ref.m_ShaderMap.end()) {
        return it->second->hash;
    } else {
        Ref<Shader> shader = CreateRef<Shader>(vertexShader, fragShader);

        // If necessary, ensure there is no hash collisions

        ref.m_ShaderMap[shader->hash] = shader;
        return shader->hash;
    }
}
}
