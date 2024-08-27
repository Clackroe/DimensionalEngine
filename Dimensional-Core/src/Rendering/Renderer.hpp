#ifndef DM_RENDERER_HPP
#define DM_RENDERER_HPP

#include "Input/Input.hpp"
#include "Rendering/ElementBuffer.hpp"
#include <Rendering/FrameBuffer.hpp>
#include <buffer.hpp>
#include <core.hpp>

namespace Dimensional {

// Forward Declare
class Shader;
class Texture;
class Mesh;
class Model;
//

static FrameBufferSettings fbs = {
    1280,
    720,
};

class DMCORE_API Renderer {
public:
    Renderer() {};
    ~Renderer() {};

    void Init()
    {
        if (s_RendererRef) {
            DM_CORE_WARN("Renderer Already initialized!");
            return;
        }
        generatePrimitives();
        s_RendererRef = this;
        DM_CORE_INFO("Renderer Initialized.")
        m_FrameBuffer = new FrameBuffer(fbs);
    };

    // Factories
    static Ref<Shader> createShader(std::string vertexShader, std::string fragShader);
    static Ref<Shader> createShader(std::string path);
    static Ref<Texture> createTexture(std::string path, bool retainInMemory);
    static void createMesh();
    static void createModel();
    //

    // Rendering
    static void renderVAO(VertexArray vao, u32 triangleCount, Ref<Shader>& shader);
    static void renderVAO(const VertexArray& vao, const ElementBuffer& eb, const Ref<Shader>& shader);
    static void renderSphere(Ref<Shader>& shader);
    static void renderCube(Ref<Shader>& shader);
    static void renderMesh(Mesh& mesh, Ref<Shader>& shader);
    static void renderModel(Model& model, Ref<Shader>& shader);
    //

    static void beginScene();
    static void endScene();

    // TODO: Create a better way to deal with framebuffers
    static u32 getFrameBufferColorID() { return m_GetRenderer().m_FrameBuffer->m_ColorGLId; };
    //

    // -----

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

    void generatePrimitives();
    void generateSphere();
    void generateCube();
    // TODO: Add to Mesh map
    Ref<VertexArray> sphereVao;
    Ref<ElementBuffer> sphereEb;

    Ref<VertexArray> cubeVao;
    Ref<ElementBuffer> cubeEb;

    //

    UMap<std::string, Ref<Shader>> m_ShaderMap;
    UMap<std::string, Ref<Texture>> m_TextureMap;

    FrameBuffer* m_FrameBuffer;

    static Renderer* s_RendererRef;
};
}

#endif
