#ifndef DM_RENDERER_HPP
#define DM_RENDERER_HPP

#include <buffer.hpp>
#include <core.hpp>

namespace Dimensional {

// Forward Declare
class Shader;
class Texture;
//

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

    // Factories
    static Ref<Shader> createShader(std::string vertexShader, std::string fragShader);
    static Ref<Texture> createTexture(std::string path, bool retainInMemory);
    static void createMesh();
    static void createModel();
    //

    // Rendering
    static void renderVAO(VertexArray vao, u32 triangleCount, Ref<Shader>& shader);
    static void renderVAO(const VertexArray& vao, const ElementBuffer& eb, const Ref<Shader>& shader);

    //

    static Ref<Shader> getShader(const std::string& name)
    {
        auto it = m_GetRenderer().m_ShaderMap.find(name);
        if (it != m_GetRenderer().m_ShaderMap.end()) {
            return it->second;
        } else {
            DM_CORE_WARN("Texture with name \"{0}\" not found", name);
            return nullptr;
        }
    }
    static Ref<Texture> getTexture(const std::string& name)
    {
        auto it = m_GetRenderer().m_TextureMap.find(name);
        if (it != m_GetRenderer().m_TextureMap.end()) {
            return it->second;
        } else {
            DM_CORE_WARN("Texture with name \"{0}\" not found", name);
            return nullptr;
        }
    }

private:
    static Renderer& m_GetRenderer() { return *s_RendererRef; }

    UMap<std::string, Ref<Shader>> m_ShaderMap;
    UMap<std::string, Ref<Texture>> m_TextureMap;

    static Renderer* s_RendererRef;
};
}

#endif
