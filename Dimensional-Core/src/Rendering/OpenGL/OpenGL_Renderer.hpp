#ifndef OPENGL_RENDERER
#define OPENGL_RENDERER

#include "Core/Window.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/SubMeshManager.hpp"
namespace Dimensional {

struct DrawElementsIndirectCommand {
    uint count;
    uint instanceCount;
    uint firstIndex;
    int baseVertex;
    uint baseInstance;
};

namespace OpenGLRenderer {

    void Init(Window& window);
    void Shutdown();

    void SetWindowHints();

    void SetClearColor(glm::vec3 color);
    void ClearScreen(ClearBuffer cb);

    void DrawIndexed(Ref<VAO> vao, Ref<Shader> shader);

    SubMeshIndexData AddSubMesh(const SubMeshData& data);

    void ImGuiDrawTexture(Ref<Texture2D> tex, u32 width, u32 height);

    RendererAPI GetAPI();
};

};

#endif // OPENGL_RENDERER
