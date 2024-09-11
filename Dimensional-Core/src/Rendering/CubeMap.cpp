#include "Core/Assets/AssetManager.hpp"
#include <Rendering/CubeMap.hpp>
#include <Rendering/Renderer.hpp>
#include <glad.h>
namespace Dimensional {
Ref<Shader> CubeMap::s_EquirectToCubeMap = nullptr;
u32 CubeMap::s_CaptureFBO = 0;
u32 CubeMap::s_DepthId = 0;

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

CubeMap::CubeMap(std::string path, u32 w, u32 h)
    : m_Width(w)
    , m_Height(h)
{
    if (!s_EquirectToCubeMap) {
        s_EquirectToCubeMap = AssetManager::getShader("EquirectToCubeMap");
    }
    if (s_CaptureFBO == 0) {
        glGenFramebuffers(1, &s_CaptureFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, s_CaptureFBO);

        glCreateTextures(GL_TEXTURE_2D, 1, &s_DepthId);
        glBindTexture(GL_TEXTURE_2D, s_DepthId);
        DM_CORE_INFO("W: {0}, H:{1}", m_Width, m_Height);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, s_DepthId, 0);
        checkFrameBufferStatus();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    m_EnvironmentTexture = AssetManager::loadTexture(path, false);
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

void CubeMap::createMainTexture()
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
void CubeMap::bind(u32 slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_GLId);
}

void CubeMap::renderToCubeMap()
{
    s_EquirectToCubeMap->use();
    s_EquirectToCubeMap->setInt("uEquirectMap", 0);
    s_EquirectToCubeMap->setMat4("uProj", captureProjection);
    m_EnvironmentTexture->bind(0);

    glBindFramebuffer(GL_FRAMEBUFFER, s_CaptureFBO);
    glViewport(0, 0, m_Width, m_Height);

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_GLId);
    for (u32 i = 0; i < 6; i++) {
        s_EquirectToCubeMap->setMat4("uView", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_GLId, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Renderer::renderCube(s_EquirectToCubeMap);
    }
    DM_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "CubeMap Framebuffer Failed To Create And Is Incomplete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
