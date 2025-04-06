#ifndef RENDER_PASS_MANAGER_AGNOST_HPP
#define RENDER_PASS_MANAGER_AGNOST_HPP

#include "Core/UUID.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/OpenGL/OpenGL_RenderPass.hpp"
#include "Rendering/RenderPass.hpp"
#include <variant>
namespace Dimensional {

struct GraphicsRenderPass {
    std::variant<std::monostate, OpenGLRenderPass> glRenderPass;
};

struct RenderPassManager {

    static UUID CreateGPUBuffer(const RenderPassData& data);

    static void Start(UUID id);

    static void Submit(UUID id, Ref<Mesh> mesh);

    static void Submit(UUID id, Ref<SubMesh> sMesh);

    static void End(UUID id);

    static void Destroy(UUID id);

private:
    static UMap<UUID, GraphicsRenderPass> s_GraphicsRenderPass;
};

}
#endif
