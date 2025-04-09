#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <Core/Window.hpp>
namespace Dimensional {

struct RendererInfo {
    Window& window;
};

struct Renderer {

    static void Init(const RendererInfo& data);

    static void Shutdown();
};
}

#endif // RENDERER_HPP
