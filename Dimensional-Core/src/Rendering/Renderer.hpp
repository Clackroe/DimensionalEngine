#ifndef DM_RENDERER_HPP
#define DM_RENDERER_HPP

#include <core.hpp>

// Forward Declare
class Shader;

namespace Dimensional {
class Renderer {
public:
    Renderer() {};
    ~Renderer() {};

    void Init()
    {
        if (s_RendererRef) {
            DM_CORE_WARN("Renderer Already initialized!");
            return;
        }
        s_RendererRef = this;
        DM_CORE_INFO("Renderer Initialized.")
    };

    Ref<Shader> getCurrentShader() const { return m_CurrentShader; };

    // Factories
    static Hash createShader(std::string vertexShader, std::string fragShader);
    static void createTexture();
    static void createMesh();
    static void createModel();
    //

    static void setShader(Hash shaderHash)
    {
        Renderer& ref = m_GetRenderer();
        auto it = ref.m_ShaderMap.find(shaderHash);
        if (it != ref.m_ShaderMap.end()) {
            ref.m_CurrentShader = it->second;
        } else {
            DM_CORE_WARN("Shader with hash \"{0}\" not found", shaderHash);
        }
    };

private:
    static Renderer& m_GetRenderer() { return *s_RendererRef; }
    Ref<Shader> m_CurrentShader;
    std::unordered_map<Hash, Ref<Shader>> m_ShaderMap;

    static Renderer* s_RendererRef;
};
}

#endif
