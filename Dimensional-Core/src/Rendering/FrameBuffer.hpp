#ifndef DM_FRAMEBUFFER_H
#define DM_FRAMEBUFFER_H
#include <core.hpp>
namespace Dimensional {

enum DMCORE_API FramebufferAttachmentFormat {
    None = 0,
    RGBA8,
    RGBA16F,
    RGBA32F,
    // TODO: Support more formats if needed

    DEPTH24STENCIl8,
    Depth = DEPTH24STENCIl8
};

struct DMCORE_API FramebufferAttachmentSettings {
    FramebufferAttachmentSettings() = default;
    FramebufferAttachmentSettings(FramebufferAttachmentFormat fmt)
        : attachmentFormat(fmt)
    {
    }

    FramebufferAttachmentFormat attachmentFormat = FramebufferAttachmentFormat::None;
};

struct DMCORE_API FrameBufferSettings {
    u32 width, height;

    std::vector<FramebufferAttachmentSettings> attachmentsList;

    FrameBufferSettings(u32 w, u32 h, std::initializer_list<FramebufferAttachmentSettings> attachments)
        : width(w)
        , height(h)
        , attachmentsList(attachments)
    {
    }
};

class DMCORE_API FrameBuffer {
public:
    FrameBuffer(const FrameBufferSettings& settings);
    ~FrameBuffer();

    void Bind();
    void Unbind();

    void bindAttachment(u32 index, u32 slot);
    void bindDephAttachment(u32 slot);

    u32 getAttachmentID(u32 index);
    u32 getDepthID();

    void setSize(u32 width, u32 height);

private:
    u32 m_GLId = 0;

    std::vector<FramebufferAttachmentSettings> m_ColorAttachmentSettings;
    FramebufferAttachmentSettings m_DepthAttachment = FramebufferAttachmentFormat::None;

    std::vector<u32> m_AttachmentIDs;
    u32 m_DepthID = 0;

    FrameBufferSettings m_Settings;

    void Rebuild();
};
}
#endif
