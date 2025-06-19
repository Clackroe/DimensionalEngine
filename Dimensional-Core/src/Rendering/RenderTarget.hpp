#ifndef DM_RENDERTARGET_HPP
#define DM_RENDERTARGET_HPP
#include "Rendering/Texture.hpp"
#include "nvrhi/nvrhi.h"
#include <vector>
namespace Dimensional {

struct RenderTargetAttachmentInfo {
    TextureFormat format = TextureFormat::RGBA;
    bool isHDR = false;
    u32 sampleCount = 1;
    bool storage = false;
};

struct RenderTargetCreateInfo {
    u32 width, height;

    std::vector<RenderTargetAttachmentInfo> colorAttachments;

    TextureFormat depthFormat = TextureFormat::NONE;
    u32 depthSampleCount;

    bool clearColor = true;
    bool clearDepth = true;
    glm::vec4 clearColorValue = { 0, 1.0, 0, 1.0 };
    float clearDepthValue = 1.0f;
    u8 clearStencilValue = 0;

    std::string debugName = "Unamed RenderTarget";
};

class RenderTarget {
public:
    static Ref<RenderTarget> Create(const RenderTargetCreateInfo& info);

    void Bind(nvrhi::CommandListHandle cmd, nvrhi::GraphicsState& state);

    void Resize(u32 width, u32 height);

    nvrhi::FramebufferHandle GetFramebuffer() const { return m_Framebuffer; }

    Ref<Texture2D> GetColorAttachment(u32 index = 0) const;
    Ref<Texture2D> GetDepthAttachment() const { return m_DepthTexture; }

    u32 GetWidth() const { return m_Width; }
    u32 GetHeight() const { return m_Height; }
    u32 GetColorAttachmentCount() const { return (u32)m_ColorTextures.size(); }

    const std::string& GetDebugName() const { return m_DebugName; }

private:
    RenderTarget() = default;

    bool Init(nvrhi::DeviceHandle dev, const RenderTargetCreateInfo& info);
    bool CreateFramebuffer(nvrhi::DeviceHandle dev);

    u32 m_Width = 0;
    u32 m_Height = 0;
    std::string m_DebugName;

    nvrhi::FramebufferHandle m_Framebuffer;

    std::vector<Ref<Texture2D>> m_ColorTextures;
    Ref<Texture2D> m_DepthTexture;

    nvrhi::Color m_ClearColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f);
    float m_ClearDepth = 1.0f;
    u8 m_ClearStencil = 0;

    RenderTargetCreateInfo m_CreateInfo;
};
}
#endif // DM_RENDERTARGET_HPP
