#ifndef AGNOST_RENDERER_HPP
#define AGNOST_RENDERER_HPP

#include "Rendering/Shader.hpp"
#include "Rendering/VAO.hpp"
namespace Dimensional {

class Window;

enum class GraphicsAPI {
    OPENGL,
    UNKOWN
};

struct RendererAPI {
    std::function<void(Window& window)> Init;
    std::function<void(void)> Shutdown;
    GraphicsAPI api;
};

enum class ClearBuffer {
    NONE,
    COLOR,
    DEPTH,
    BOTH,
    DEFAULT = BOTH
};

struct Renderer {

    static void Init(GraphicsAPI api, Window& window);
    static void Shutdown();

    static void SetAPI(GraphicsAPI api);
    static void SetWindowHints();
    static void ClearScreen(ClearBuffer cb);

    static void DrawIndexed(Ref<VAO> vao, Ref<Shader> shader);

    static void SetClearColor(glm::vec3 color);

    static void PushCommand();

private:
    static RendererAPI s_API;
};

}
#endif // AGNOST_RENDERER_HPP
