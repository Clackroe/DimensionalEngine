#ifndef OPENGL_RENDERPASS_HPP
#define OPENGL_RENDERPASS_HPP
#include "Rendering/Mesh.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/SubMesh.hpp"
namespace Dimensional {

struct OpenGLRenderPass {
    static OpenGLRenderPass Create(const RenderPassData& data);

    void Start();

    void Submit(Ref<Mesh> mesh);
    void Submit(Ref<SubMesh> sMesh);

    void End();

    void Destroy();

private:
};
};
#endif // OPENGL_RENDERPASS_HPP
