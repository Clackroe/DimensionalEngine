#include "Core/Assets/AssetManager.hpp"
#include "Log/log.hpp"
#include <Rendering/IBLMap.hpp>
#include <Rendering/Renderer.hpp>
#include <glad.h>

namespace Dimensional {
Ref<Shader> IBLMap::s_ConvShader = nullptr;
u32 IBLMap::s_CaptureFBOIBL = 0;
u32 IBLMap::s_DepthIdIBL = 0;

static void checkFrameBufferStatus()
{
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        switch (status) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            DM_CORE_ERROR("Framebuffer incomplete: Attachment is missing or incomplete.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            DM_CORE_ERROR("Framebuffer incomplete: No images are attached to the framebuffer.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            DM_CORE_ERROR("Framebuffer incomplete: Draw buffer.");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            DM_CORE_ERROR("Framebuffer incomplete: Read buffer.");
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            DM_CORE_ERROR("Framebuffer incomplete: Unsupported format.");
            break;
        default:
            DM_CORE_ERROR("Framebuffer incomplete: Unknown error.");
            break;
        }
    }
}

IBLMap::IBLMap(Ref<CubeMap> ref)
{
    m_Width = (u32)(ref->m_Width / 16);
    m_Height = (u32)(ref->m_Height / 16);
    m_CubeMap = ref;
    build();
}

void IBLMap::build()
{

    if (!s_ConvShader) {
        s_ConvShader = AssetManager::getShader("CubeMapConv");
    }
    if (s_CaptureFBOIBL == 0) {
        glGenFramebuffers(1, &s_CaptureFBOIBL);
        glBindFramebuffer(GL_FRAMEBUFFER, s_CaptureFBOIBL);

        glCreateTextures(GL_TEXTURE_2D, 1, &s_DepthIdIBL);
        glBindTexture(GL_TEXTURE_2D, s_DepthIdIBL);
        DM_CORE_INFO("W: {0}, H:{1}", m_Width, m_Height);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, s_DepthIdIBL, 0);
        checkFrameBufferStatus();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    createMainTexture();
    renderToCubeMap();
}

static glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
static glm::mat4 captureViews[] = {
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
};

void IBLMap::createMainTexture()
{
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_GLId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_GLId);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
void IBLMap::bind(u32 slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_GLId);
}

void IBLMap::renderToCubeMap()
{
    s_ConvShader->use();
    s_ConvShader->setInt("uMap", 0);
    s_ConvShader->setMat4("uProj", captureProjection);
    m_CubeMap->bind(0);

    glViewport(0, 0, m_Width, m_Height);
    glBindFramebuffer(GL_FRAMEBUFFER, s_CaptureFBOIBL);

    for (u32 i = 0; i < 6; i++) {
        s_ConvShader->setMat4("uView", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_GLId, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Renderer::renderCube(s_ConvShader);
    }
    DM_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "IBLMap Framebuffer Failed To Create And Is Incomplete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
