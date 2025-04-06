#include "Rendering/OpenGL/OpenGL_RenderPass.hpp"
namespace Dimensional {
OpenGLRenderPass OpenGLRenderPass::Create(const RenderPassData& data)
{
}

void OpenGLRenderPass::Start() { }

void OpenGLRenderPass::Submit(Ref<Mesh> mesh) { }

void OpenGLRenderPass::Submit(Ref<SubMesh> sMesh) { }

void OpenGLRenderPass::End() { }

void OpenGLRenderPass::Destroy() { }
}
