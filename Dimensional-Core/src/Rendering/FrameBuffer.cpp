#include "Log/log.hpp"
#include "core.hpp"
#include <Rendering/FrameBuffer.hpp>
#include <glad.h>
namespace Dimensional {

static void attachColorTexture(u32 glId, GLenum internalFormat, GLenum format, u32 w, u32 h, u32 index)
{

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, glId, 0);
}

static void attachDepthTexture(u32 glId, GLenum format, GLenum type, u32 w, u32 h)
{

    glTexStorage2D(GL_TEXTURE_2D, 1, format, w, h);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glFramebufferTexture2D(GL_FRAMEBUFFER, type, GL_TEXTURE_2D, glId, 0);
}

FrameBuffer::FrameBuffer(const FrameBufferSettings& settings)
    : m_Settings(settings)
{
    for (auto& attachment : m_Settings.attachmentsList) {
        if (attachment.attachmentFormat != Depth) {
            m_ColorAttachmentSettings.emplace_back(attachment);
        } else {
            m_DepthAttachment = attachment;
        }
    }
    Rebuild();
}
FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_GLId);
}

void FrameBuffer::setSize(u32 width, u32 height)
{
    m_Settings.width = width;
    m_Settings.height = height;
    Rebuild();
}

void FrameBuffer::Rebuild()
{

    if (m_GLId) {
        // Delete and rebuild
        glDeleteFramebuffers(1, &m_GLId);
        glDeleteTextures((int)m_AttachmentIDs.size(), m_AttachmentIDs.data());
        glDeleteTextures(1, &m_DepthID);
        m_ColorAttachmentSettings.clear();
        m_DepthAttachment = FramebufferAttachmentFormat::None;
    }

    glCreateFramebuffers(1, &m_GLId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_GLId);

    if (!m_ColorAttachmentSettings.empty()) {

        m_AttachmentIDs.resize(m_ColorAttachmentSettings.size());
        glCreateTextures(GL_TEXTURE_2D, m_ColorAttachmentSettings.size(), m_AttachmentIDs.data());
        for (u32 i = 0; i < m_ColorAttachmentSettings.size(); i++) {
            glBindTexture(GL_TEXTURE_2D, m_AttachmentIDs[i]);
            switch (m_ColorAttachmentSettings[i].attachmentFormat) {
            case (FramebufferAttachmentFormat::RGBA8):
                attachColorTexture(m_AttachmentIDs[i], GL_RGBA8, GL_RGBA, m_Settings.width, m_Settings.height, i);
                break;
            case (FramebufferAttachmentFormat::RGBA16F):
                attachColorTexture(m_AttachmentIDs[i], GL_RGBA16F, GL_RGBA, m_Settings.width, m_Settings.height, i);
                break;
            case (FramebufferAttachmentFormat::RGBA32F):
                attachColorTexture(m_AttachmentIDs[i], GL_RGBA32F, GL_RGBA, m_Settings.width, m_Settings.height, i);
                break;
            default:
                DM_CORE_ASSERT(false, "Ivalid Framebuffer Color Attachment Format.");
                break;
            }
        }
    }

    if (m_DepthAttachment.attachmentFormat != FramebufferAttachmentFormat::None) {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthID);
        glBindTexture(GL_TEXTURE_2D, m_DepthID);

        // Currently only support one type of depth texture
        attachDepthTexture(m_DepthID, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Settings.width, m_Settings.height);
    }

    if (m_ColorAttachmentSettings.size() > 1) {
        DM_CORE_ASSERT(m_AttachmentIDs.size() <= 5, "Too Many FrameBuffer Attachments Provided")
        constexpr GLenum buffers[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
        glDrawBuffers(static_cast<int>(m_AttachmentIDs.size()), buffers);
    } else if (m_AttachmentIDs.empty()) {
        glDrawBuffer(GL_NONE);
    }

    DM_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer Failed To Create And Is Incomplete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FrameBuffer::bindAttachment(u32 index, u32 slot)
{
    glBindTextureUnit(m_AttachmentIDs[index], slot);
}
void FrameBuffer::bindDephAttachment(u32 slot)
{
    glBindTextureUnit(m_DepthID, slot);
}

u32 FrameBuffer::getAttachmentID(u32 index)
{
    return m_AttachmentIDs[index];
}
u32 FrameBuffer::getDepthID()
{
    if (m_DepthID) {
        return m_DepthID;
    }
    DM_CORE_WARN("Tried to get depth id from a framebuffer that does not have a depth texture attached.");
    return 0;
}

void FrameBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_GLId);
    glViewport(0, 0, m_Settings.width, m_Settings.height);
}
void FrameBuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
