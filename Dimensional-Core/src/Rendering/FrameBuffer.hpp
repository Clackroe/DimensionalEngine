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
    DEPTHCOMPONENT24,
    Shadow = DEPTHCOMPONENT24,
    Depth = DEPTH24STENCIl8
};
enum DMCORE_API FramebufferAttachmentType {
    TEXTURE_2D,
    ARRAY_2D,
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

    FramebufferAttachmentType bufferType = FramebufferAttachmentType::TEXTURE_2D;

    std::vector<FramebufferAttachmentSettings> attachmentsList;

    u32 layers = 1;

    FrameBufferSettings(u32 w, u32 h, std::initializer_list<FramebufferAttachmentSettings> attachments, FramebufferAttachmentType type = TEXTURE_2D, u32 layers = 1, bool cube = false)
        : width(w)
        , height(h)
        , bufferType(type)
        , attachmentsList(attachments)
        , layers(layers)
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
    void bindDepthAttachment(u32 slot);

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
