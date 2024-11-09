#ifndef DM_RENDERERAPI_HPP
#define DM_RENDERERAPI_HPP

#include <Rendering/Material.hpp>
#include <Rendering/Mesh.hpp>
#include <Rendering/Model.hpp>
#include <Rendering/Shader.hpp>

#include <buffer.hpp>

typedef void* (*GLADloadproc)(const char* name);

namespace Dimensional {
enum class DepthFunc {
    NEVER,
    LESS,
    EQUAL,
    LEQUAL,
    GREATER,
    NOTEQUAL,
    GEQUAL,
    ALWAYS,
    DEFAULT = LESS
};

enum class BlendFunc {

};

enum class FaceCulling {
    FRONTANDBACK,
    FRONT,
    BACK,
    DEFAULT = BACK
};

enum class ClearBuffer {
    NONE,
    COLOR,
    DEPTH,
    BOTH,
    DEFAULT = BOTH
};
// Potentially factor out into multiple implementations for different render APIS
// Currently, I only plan to support OpenGL, and maybe Vulkan in the distant future.
// Vulkan will warrent a rewrite anyway.
class RendererAPI {
public:
    static RendererAPI& getInstance()
    {
        static RendererAPI api;
        return api;
    };

    void Init(GLADloadproc proc);

    void beginFrame();
    void endFrame();

    void setClearColor(const glm::vec4& color);
    void clearBuffer(ClearBuffer cb = ClearBuffer::DEFAULT);

    void renderArrays(VertexArray vao, u32 triangleCount, Ref<Shader>& shader);
    void renderIndexed(VertexArray& vao, ElementBuffer& eb, const Ref<Shader>& shader);

    void setDepthFunc(DepthFunc f);
    void setCulling(FaceCulling cull);
    // void setBlendFunc(BlendFunc f);

    void enableDepthTest();
    void enableBlending();
    void enableCulling();

    void disableDepthTest();
    void disableBlending();
    void disableCulling();

private:
};

}
#endif
