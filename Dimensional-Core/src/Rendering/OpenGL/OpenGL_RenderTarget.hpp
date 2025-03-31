#ifndef OPENGL_RENDER_TARGET
#define OPENGL_RENDER_TARGET
#include "Rendering/RenderTarget.hpp"
#include "Rendering/Texture2D.hpp"
#include <vector>
namespace Dimensional {

struct OpenGLRenderTarget {

    ~OpenGLRenderTarget() {};

    static OpenGLRenderTarget Create(const RenderTargetData& data);

    void Bind();
    void UnBind();

    void SetSize(u32 width, u32 height);
    void ReBuild();

    void BindAttachment(u32 index, u32 slot);

    void Destroy();

    u32 m_GLID;
    std::vector<Ref<Texture2D>> m_ColorBuffs;
    Ref<Texture2D> m_Depth = nullptr;

    RenderTargetData m_Data;
};
}

#endif // OPENGL_RENDER_TARGET
