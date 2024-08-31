#ifndef DM_FRAMEBUFFER_H
#define DM_FRAMEBUFFER_H
#include <core.hpp>
namespace Dimensional {
struct DMCORE_API FrameBufferSettings {
    u32 width, height;
};

class DMCORE_API FrameBuffer {
public:
    FrameBuffer(const FrameBufferSettings& settings);
    ~FrameBuffer();

    void Bind();
    void Unbind();

    void setSize(u32 width, u32 height);

    u32 m_GLId, m_ColorGLId, m_DepthGLId;

private:
    void Rebuild();

    FrameBufferSettings m_Settings;
};
}
#endif
