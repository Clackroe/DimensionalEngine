#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <Core/Window.hpp>
namespace Dimensional {

enum class RESOURCE_DOMAIN {
    CONSTANT = 0,
    FRAME = 1,
    PIPELINE = 2,
    MATERIAL = 3,
};

struct RendererInfo {
    Window& window;
};

struct Renderer {

    static void Init(const RendererInfo& data);

    static void Shutdown();
};
}

#endif // RENDERER_HPP
