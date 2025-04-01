#include "Core/Application.hpp"
#include "Rendering/Texture2D.hpp"
#include "Rendering/Texture2DManager.hpp"
#include "Rendering/TextureEnums.hpp"
#include <Rendering/OpenGL/OpenGL_RenderTarget.hpp>
#include <glad.h>
namespace Dimensional {

OpenGLRenderTarget OpenGLRenderTarget::Create(const RenderTargetData& data)
{
    OpenGLRenderTarget target;
    target.m_Data = data;

    glCreateFramebuffers(1, &target.m_GLID);

    target.ReBuild();
    return target;
}

void OpenGLRenderTarget::BindAttachment(u32 index, u32 slot)
{
    DM_CORE_ASSERT(index < m_ColorBuffs.size(), "Attatchment Index Out of Range");
    m_ColorBuffs[index]->Bind(slot);
}

void OpenGLRenderTarget::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_GLID);
    glViewport(0, 0, m_Data.width, m_Data.height);
}
void OpenGLRenderTarget::UnBind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Application::getApp().getWindowDM().getWidth(), Application::getApp().getWindowDM().getHeight());
}

void OpenGLRenderTarget::SetSize(u32 width, u32 height)
{
    m_Data.width = width;
    m_Data.height = height;
    ReBuild();
}
void OpenGLRenderTarget::ReBuild()
{
    m_ColorBuffs.clear();
    for (auto& att : m_Data.attachments) {
        Texture2DData data;
        data.width = m_Data.width;
        data.height = m_Data.height;
        data.format = att.format;
        data.channels = TextureFormatToChannels.at(att.format);
        data.wrap = TextureWrapMode::ClampToEdge;
        data.filtering = TextureFiltering::Linear;
        data.generateMipMaps = false;
        Ref<Texture2D> tex = Texture2D::Create(data);
        m_ColorBuffs.push_back(tex);
    }

    m_Depth = nullptr;
    if (m_Data.depthAttachment.format != TextureFormat::NONE) {
        Texture2DData data;
        data.width = m_Data.width;
        data.height = m_Data.height;
        data.format = m_Data.depthAttachment.format;
        data.channels = TextureFormatToChannels.at(m_Data.depthAttachment.format);
        data.wrap = TextureWrapMode::ClampToBorder;
        data.filtering = TextureFiltering::Nearest;
        data.generateMipMaps = false;

        Ref<Texture2D> tex = Texture2D::Create(data);
        m_Depth = tex;
    }

    u32 index = 0;
    for (Ref<Texture2D> tex : m_ColorBuffs) {
        u32 glID = Texture2DManager::GetOpenGLTexture(tex->GetUUID()).m_GLID;

        glNamedFramebufferTexture(m_GLID, GL_COLOR_ATTACHMENT0 + index, glID, 0);
        index++;
    }
    if (m_Depth) {
        u32 glID = Texture2DManager::GetOpenGLTexture(m_Depth->GetUUID()).m_GLID;
        glNamedFramebufferTexture(m_GLID, GL_DEPTH_ATTACHMENT, glID, 0);
    }
    Bind();
    GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    DM_CORE_ASSERT(fbStatus == GL_FRAMEBUFFER_COMPLETE, "Framebuffer Failed To Create And Is Incomplete! Status Code: " + std::to_string(fbStatus));
}

Ref<Texture2D> OpenGLRenderTarget::GetAttachment(u32 index)
{
    DM_CORE_ASSERT(index < m_ColorBuffs.size(), "Attatchment Index Out of Range");
    return m_ColorBuffs[index];
}

void OpenGLRenderTarget::Destroy() { }

}
