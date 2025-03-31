
#ifndef RENDER_TARGET_MANAGER
#define RENDER_TARGET_MANAGER
#include "Core/UUID.hpp"
#include <Rendering/OpenGL/OpenGL_RenderTarget.hpp>
#include <variant>

namespace Dimensional {

// Using Variant here because the glTarget has non-trivial members
struct GraphicsRenderTarget {
    std::variant<std::monostate, OpenGLRenderTarget> glTarget;
};

struct RenderTargetManager {

    static UUID CreateRenderTarget(const RenderTargetData& data);

    static void Bind(UUID id);
    static void UnBind(UUID id);

    static void SetSize(UUID id, u32 width, u32 height);
    static void ReBuild(UUID id);

    static void BindAttachment(UUID id, u32 index, u32 slot);

    static void Destroy(UUID id);

private:
    static UMap<UUID, GraphicsRenderTarget> s_RTMap;
};

}
#endif // RENDER_TARGET_MANAGER
