#include "core.hpp"
#include <Rendering/FrameBuffer.hpp>
#include <glad.h>

namespace Dimensional {

static GLenum fbTypeToGLType(FramebufferAttachmentType type)
{
    switch (type) {
    case TEXTURE_2D:
        return GL_TEXTURE_2D;
    case ARRAY_2D:
        return GL_TEXTURE_2D_ARRAY;
    default:
        DM_CORE_ASSERT(false, "Unknown Framebuffer Attachment Type");
        return 0;
    }
}

static void attachColorTexture(u32 glId, GLenum internalFormat, GLenum format, u32 w, u32 h, u32 index, GLenum target = GL_TEXTURE_2D, u32 layers = 1)
{
    if (target == GL_TEXTURE_2D_ARRAY) {
        glTexImage3D(target, 0, internalFormat, w, h, layers, 0, format, GL_UNSIGNED_BYTE, nullptr);
    } else {
        glTexImage2D(target, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, nullptr);
    }

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (target == GL_TEXTURE_2D_ARRAY) {
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, glId, 0, index);
    } else {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, target, glId, 0);
    }
}

static void attachDepthTexture(u32 glId, GLenum format, GLenum attachmentType, u32 w, u32 h, u32 layers = 1, GLenum target = GL_TEXTURE_2D)
{
    if (target == GL_TEXTURE_2D_ARRAY) {
        glTexStorage3D(target, 1, format, w, h, layers);
        glFramebufferTexture(GL_FRAMEBUFFER, attachmentType, glId, 0);
    } else {
        glTexStorage2D(target, 1, format, w, h);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, target, glId, 0);
    }

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
}

FrameBuffer::FrameBuffer(const FrameBufferSettings& settings)
    : m_Settings(settings)
    , m_GLId(0)
    , m_DepthID(0)
{
    for (auto& attachment : m_Settings.attachmentsList) {
        if (attachment.attachmentFormat != Depth && attachment.attachmentFormat != Shadow) {
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
        glDeleteFramebuffers(1, &m_GLId);
        glDeleteTextures(static_cast<int>(m_AttachmentIDs.size()), m_AttachmentIDs.data());
        glDeleteTextures(1, &m_DepthID);
        m_AttachmentIDs.clear();
    }

    glCreateFramebuffers(1, &m_GLId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_GLId);

    GLenum type = fbTypeToGLType(m_Settings.bufferType);
    u32 layers = m_Settings.layers;

    if (!m_ColorAttachmentSettings.empty()) {
        m_AttachmentIDs.resize(m_ColorAttachmentSettings.size());
        glCreateTextures(type, m_ColorAttachmentSettings.size(), m_AttachmentIDs.data());

        for (u32 i = 0; i < m_ColorAttachmentSettings.size(); i++) {
            glBindTexture(type, m_AttachmentIDs[i]);

            switch (m_ColorAttachmentSettings[i].attachmentFormat) {
            case FramebufferAttachmentFormat::RGBA8:
                attachColorTexture(m_AttachmentIDs[i], GL_RGBA8, GL_RGBA, m_Settings.width, m_Settings.height, i, type, layers);
                break;
            case FramebufferAttachmentFormat::RGBA16F:
                attachColorTexture(m_AttachmentIDs[i], GL_RGBA16F, GL_RGBA, m_Settings.width, m_Settings.height, i, type, layers);
                break;
            case FramebufferAttachmentFormat::RGBA32F:
                attachColorTexture(m_AttachmentIDs[i], GL_RGBA32F, GL_RGBA, m_Settings.width, m_Settings.height, i, type, layers);
                break;
            default:
                DM_CORE_ASSERT(false, "Invalid Framebuffer Color Attachment Format.");
                break;
            }
            glBindTexture(type, 0);
        }
    }

    if (m_DepthAttachment.attachmentFormat != FramebufferAttachmentFormat::None) {
        glCreateTextures(type, 1, &m_DepthID);
        glBindTexture(type, m_DepthID);

        switch (m_DepthAttachment.attachmentFormat) {
        case DEPTH24STENCIL8:
            attachDepthTexture(m_DepthID, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Settings.width, m_Settings.height, layers, type);
            break;
        case DEPTHCOMPONENT32F:
            attachDepthTexture(m_DepthID, GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, m_Settings.width, m_Settings.height, layers, type);
            break;
        default:
            DM_CORE_WARN("COLOR BUFFER ATTACHMENT MADE IT INTO THE DEPTH ATTACHMENT");
            return;
        }
        glBindTexture(type, 0);
    }

    if (m_ColorAttachmentSettings.size() > 1) {
        DM_CORE_ASSERT(m_AttachmentIDs.size() <= 5, "Too Many FrameBuffer Attachments Provided");
        std::vector<GLenum> buffers(m_AttachmentIDs.size());
        for (u32 i = 0; i < m_AttachmentIDs.size(); i++) {
            buffers[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers(static_cast<int>(m_AttachmentIDs.size()), buffers.data());
    } else if (m_ColorAttachmentSettings.size() == 0) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    DM_CORE_ASSERT(fbStatus == GL_FRAMEBUFFER_COMPLETE, "Framebuffer Failed To Create And Is Incomplete! Status Code: " + std::to_string(fbStatus));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bindAttachment(u32 index, u32 slot)
{
    glBindTextureUnit(slot, m_AttachmentIDs[index]);
}

void FrameBuffer::bindDepthAttachment(u32 slot)
{
    glBindTextureUnit(slot, m_DepthID);
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
