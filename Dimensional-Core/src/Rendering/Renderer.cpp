#include "core.hpp"
#include <Core/Shader.hpp>
#include <Rendering/Renderer.hpp>

namespace Dimensional {

Hash Renderer::createShader(std::string vertexShader, std::string fragShader)
{

    Hash t = std::hash<std::string> {}(vertexShader + fragShader);
    auto it = m_ShaderMap.find(t);
    if (it != m_ShaderMap.end()) {
        return it->second->hash;
    } else {
        Ref<Shader> shader = CreateRef<Shader>(vertexShader, fragShader);

        // If necessary, ensure there is no hash collisions

        m_ShaderMap[shader->hash] = shader;
        return shader->hash;
    }
}
}
