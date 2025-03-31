#ifndef OPENGL_RENDERER
#define OPENGL_RENDERER

#include "Core/Window.hpp"
#include "Rendering/Renderer.hpp"
namespace Dimensional {

namespace OpenGLRenderer {

    void Init(Window& window);
    void Shutdown();

    void SetWindowHints();

    void SetClearColor(glm::vec3 color);
    void ClearScreen(ClearBuffer cb);

    void DrawIndexed(Ref<VAO> vao, Ref<Shader> shader);

    RendererAPI GetAPI();
};

};

#endif // OPENGL_RENDERER
