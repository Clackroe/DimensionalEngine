

#include <Rendering/Renderer.hpp>

namespace Dimensional {

struct RendererData {
};

static RendererData s_RendererInstance;

void Renderer::Init(const RendererInfo& data)
{
    // TODO: Conditionally create device
}

void Renderer::Shutdown()
{
}

}
