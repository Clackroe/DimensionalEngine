#include "Rendering/Texture.hpp"
#include "nvrhi/nvrhi.h"
#include "nvrhi/utils.h"
#include <Core/Application.hpp>
#include <Rendering/RenderTarget.hpp>

namespace Dimensional {

Ref<RenderTarget> RenderTarget::Create(const RenderTargetCreateInfo& info)
{
    auto dev = Application::getDevice();

    Ref<RenderTarget> rt = Ref<RenderTarget>(new RenderTarget());

    if (!rt->Init(dev, info)) {
        DM_CORE_ERROR("Failed to create RenderTarget: {}", info.debugName);
        return nullptr;
    }

    return rt;
}

bool RenderTarget::Init(nvrhi::DeviceHandle dev, const RenderTargetCreateInfo& info)
{
    if (!dev) {
        DM_CORE_ERROR("Invalid device handle");
        return false;
    }

    if (info.colorAttachments.empty() && info.depthFormat != TextureFormat::NONE) {
        DM_CORE_ERROR("RenderTarget must have at least one color or depth attachment");
        return false;
    }

    m_Width = info.width;
    m_Height = info.height;
    m_DebugName = info.debugName;
    m_CreateInfo = info;

    m_ClearColor = nvrhi::Color(info.clearColorValue.r, info.clearColorValue.g, info.clearColorValue.b, info.clearColorValue.a);
    m_ClearDepth = info.clearDepthValue;
    m_ClearStencil = info.clearStencilValue;

    m_ColorTextures.reserve(info.colorAttachments.size());
    for (size_t i = 0; i < info.colorAttachments.size(); ++i) {
        const auto& attachmentInfo = info.colorAttachments[i];

        TextureCreateInfo texInfo;
        texInfo.format = attachmentInfo.format;
        texInfo.isHDR = attachmentInfo.isHDR;
        texInfo.width = m_Width;
        texInfo.height = m_Height;
        texInfo.sampleCount = attachmentInfo.sampleCount;
        texInfo.generateMipmaps = false;
        texInfo.storage = attachmentInfo.storage;
        texInfo.isRenderTarget = true;
        texInfo.debugName = m_DebugName + "_Color" + std::to_string(i);

        auto colorTexture = Texture2D::Create(texInfo);
        if (!colorTexture) {
            DM_CORE_ERROR("Failed to create color attachment {} for RenderTarget {}", i, m_DebugName);
            return false;
        }

        m_ColorTextures.push_back(colorTexture);
    }

    if (info.depthFormat != TextureFormat::NONE) {
        TextureCreateInfo depthInfo;
        depthInfo.format = info.depthFormat;
        depthInfo.width = m_Width;
        depthInfo.height = m_Height;
        depthInfo.sampleCount = info.depthSampleCount;
        depthInfo.generateMipmaps = false;
        depthInfo.storage = false;
        depthInfo.isRenderTarget = true;
        depthInfo.debugName = m_DebugName + "_Depth";

        m_DepthTexture = Texture2D::Create(depthInfo);
        if (!m_DepthTexture) {
            DM_CORE_ERROR("Failed to create depth attachment for RenderTarget {}", m_DebugName);
            return false;
        }
    }

    if (!CreateFramebuffer(dev)) {
        DM_CORE_ERROR("Failed to create framebuffer for RenderTarget {}", m_DebugName);
        return false;
    }

    return true;
}

void RenderTarget::Bind(nvrhi::CommandListHandle cmd, nvrhi::GraphicsState& state)
{
    if (m_CreateInfo.clearColor) {
        for (int i = 0; i < m_ColorTextures.size(); i++) {
            nvrhi::utils::ClearColorAttachment(cmd, m_Framebuffer, i, m_ClearColor);
        }
    }
    if (m_CreateInfo.clearDepth) {
        nvrhi::utils::ClearDepthStencilAttachment(cmd, m_Framebuffer, m_ClearDepth, m_ClearStencil);
    }
    state.setFramebuffer(m_Framebuffer);
    state.setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(m_Width, m_Height)));
}

bool RenderTarget::CreateFramebuffer(nvrhi::DeviceHandle dev)
{
    nvrhi::FramebufferDesc fbDesc;

    for (auto& colorTexture : m_ColorTextures) {
        nvrhi::FramebufferAttachment colorAttachment;
        colorAttachment.texture = colorTexture->GetHandle();
        colorAttachment.subresources = nvrhi::AllSubresources;
        fbDesc.addColorAttachment(colorAttachment);
    }

    if (m_DepthTexture) {
        nvrhi::FramebufferAttachment depthAttachment;
        depthAttachment.texture = m_DepthTexture->GetHandle();
        depthAttachment.subresources = nvrhi::AllSubresources;
        fbDesc.setDepthAttachment(depthAttachment);
    }

    m_Framebuffer = dev->createFramebuffer(fbDesc);
    if (!m_Framebuffer) {
        DM_CORE_ERROR("Failed to create NVRHI framebuffer for {}", m_DebugName);
        return false;
    }

    return true;
}

void RenderTarget::Resize(u32 width, u32 height)
{
    if (width == m_Width && height == m_Height) {
        return; // No change needed
    }

    auto dev = Application::getDevice();
    if (!dev) {
        DM_CORE_ERROR("Invalid device handle during resize");
        return;
    }

    m_Width = width;
    m_Height = height;

    for (auto& colorTexture : m_ColorTextures) {
        colorTexture->Resize(width, height);
    }

    if (m_DepthTexture) {
        m_DepthTexture->Resize(width, height);
    }

    if (!CreateFramebuffer(dev)) {
        DM_CORE_ERROR("Failed to recreate framebuffer after resize for RenderTarget {}", m_DebugName);
    }
}

Ref<Texture2D> RenderTarget::GetColorAttachment(u32 index) const
{
    if (index >= m_ColorTextures.size()) {
        DM_CORE_WARN("Color attachment index {} out of range for RenderTarget {}", index, m_DebugName);
        return nullptr;
    }

    return m_ColorTextures[index];
}

}
