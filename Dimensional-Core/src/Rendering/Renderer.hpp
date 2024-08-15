#ifndef DM_RENDERER_HPP
#define DM_RENDERER_HPP

#include "Core/Assets/Asset.hpp"
#include <core.hpp>

// Forward Declare
class Shader;

namespace Dimensional {
class Renderer {
public:
    Renderer() {};
    ~Renderer() {};

    void Init() {};

    Ref<Shader> getCurrentShader() const { return m_CurrentShader; };

    // Factories
    Hash createShader(std::string vertexShader, std::string fragShader);
    void createTexture();
    void createMesh();
    void createModel();
    //

    void setShader(Hash shaderHash)
    {
        auto it = m_ShaderMap.find(shaderHash);
        if (it != m_ShaderMap.end()) {
            m_CurrentShader = it->second;
        } else {
            DM_CORE_WARN("Shader with hash \"{0}\" not found", shaderHash);
        }
    };

private:
    Ref<Shader> m_CurrentShader;
    std::unordered_map<Hash, Ref<Shader>> m_ShaderMap;
};
}

#endif
