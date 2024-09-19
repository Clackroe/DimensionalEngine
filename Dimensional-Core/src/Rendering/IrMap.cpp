#include "Assets/AssetManager.hpp"
#include "Log/log.hpp"
#include <Rendering/IrMap.hpp>
#include <Rendering/Renderer.hpp>
#include <glad.h>

namespace Dimensional {
Ref<Shader> IrMap::s_ConvShader = nullptr;
u32 IrMap::s_CaptureFBOIBL = 0;
u32 IrMap::s_DepthIdIBL = 0;

IrMap::IrMap(Ref<CubeMap> ref)
{
    m_Width = (u32)(ref->m_Width / 16);
    m_Height = (u32)(ref->m_Height / 16);
    m_CubeMap = ref;
    build();
}

void IrMap::build()
{

    if (!s_ConvShader) {
        s_ConvShader = AssetManager::getShader("CubeMapConvComp");
    }
    createMainTexture();
    renderToCubeMap();
}

void IrMap::createMainTexture()
{
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_GLId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_GLId);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
void IrMap::bind(u32 slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_GLId);
}

void IrMap::renderToCubeMap()
{
    s_ConvShader->use();
    m_CubeMap->bind(0);
    glBindImageTexture(0, m_CubeMap->getID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
    glBindImageTexture(1, m_GLId, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
    s_ConvShader->dispatchCompute((m_Width + 15) / 16, (m_Height + 15) / 16, 6);
}

}
