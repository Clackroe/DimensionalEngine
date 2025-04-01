#ifndef RENDERTARGET_AGNOST_HPP
#define RENDERTARGET_AGNOST_HPP

#include "Core/UUID.hpp"
#include "Rendering/Texture2D.hpp"
#include "TextureEnums.hpp"

namespace Dimensional {

struct RenderAttachmentSpec {
    TextureFormat format;
};

struct RenderTargetData {
    u32 width, height;
    std::vector<RenderAttachmentSpec> attachments;
    RenderAttachmentSpec depthAttachment = { TextureFormat::NONE };
    bool hdr;
};

struct RenderTarget {

    static Ref<RenderTarget> Create(const RenderTargetData& data);

    void Bind();
    void UnBind();

    void SetSize(u32 width, u32 height);
    void ReBuild();

    void BindAttachment(u32 index, u32 slot);

    Ref<Texture2D> GetAttachment(u32 index);

    void Destroy();

private:
    RenderTarget() = default;
    UUID m_UUID;
};
};

#endif // RENDERTARGET_AGNOST_HPP
