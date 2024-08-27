#include <Rendering/FrameBuffer.hpp>
#include <glad.h>
namespace Dimensional {

FrameBuffer::FrameBuffer(const FrameBufferSettings& settings)
    : m_Settings(settings)
{
    Rebuild();
}
FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_GLId);
}

void FrameBuffer::Rebuild()
{
    glCreateFramebuffers(1, &m_GLId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_GLId);

    // Color
    glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorGLId);
    glBindTexture(GL_TEXTURE_2D, m_ColorGLId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Settings.width, m_Settings.height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // glFramebufferTexture2D(GL_FRAMEBUFFER, 0, false, m_ColorGLId, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorGLId, 0);

    // Depth
    glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthGLId);
    glBindTexture(GL_TEXTURE_2D, m_DepthGLId);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Settings.width, m_Settings.height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //---
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, false, m_DepthGLId, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthGLId, 0);

    DM_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
