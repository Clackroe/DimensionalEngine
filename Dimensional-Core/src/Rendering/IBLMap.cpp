#include "Asset/AssetManager.hpp"
#include "Log/log.hpp"
#include <Rendering/IBLMap.hpp>
#include <glad.h>

namespace Dimensional {
Ref<Shader> IBLMap::s_PreComp = nullptr;
Ref<Shader> IBLMap::s_BRDFComp = nullptr;

IBLMap::IBLMap(Ref<CubeMap> ref)
{
    m_CubeMap = ref;
    m_Width = (u32)(ref->m_Width / 4);
    m_Height = (u32)(ref->m_Height / 4);
    build();
}

void IBLMap::build()
{

    if (!s_PreComp) {
        s_PreComp = CreateRef<Shader>("Assets/Shaders/IBLMapPreComp.glsl", ShaderType::COMPUTE);
    }
    if (!s_BRDFComp) {
        s_BRDFComp = CreateRef<Shader>("Assets/Shaders/BRDFComp.glsl", ShaderType::COMPUTE);
    }

    createMainTexture();
    createBRDFTexture();
    renderToCubeMap();
    renderBRDF();
}
void IBLMap::createBRDFTexture()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_BRDFId);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, m_BRDFId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, m_CubeMap->m_Width, m_CubeMap->m_Height, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void IBLMap::createMainTexture()
{
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_GLId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_GLId);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
void IBLMap::bind(u32 slotIBLMap, u32 slotBRDF)
{
    glActiveTexture(GL_TEXTURE0 + slotIBLMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_GLId);

    glActiveTexture(GL_TEXTURE0 + slotBRDF);
    glBindTexture(GL_TEXTURE_2D, m_BRDFId);
}

void IBLMap::renderToCubeMap()
{
    s_PreComp->use();
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    m_CubeMap->bind(0);
    glBindImageTexture(0, m_CubeMap->getID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
    u32 mipLevels = 5;
    for (u32 mip = 0; mip < mipLevels; mip++) {
        glBindImageTexture(1, m_GLId, mip, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
        u32 mipWidth = static_cast<u32>(m_Width * std::pow(0.5, mip));
        u32 mipHeight = static_cast<u32>(m_Height * std::pow(0.5, mip));
        float roughness = (float)mip / (float)(mipLevels - 1);
        s_PreComp->setFloat("roughness", roughness);
        s_PreComp->dispatchCompute((mipWidth + 15) / 16, (mipHeight + 15) / 16, 6);
    }
}
void IBLMap::renderBRDF()
{
    s_BRDFComp->use();
    glBindImageTexture(0, m_BRDFId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG16F);
    s_BRDFComp->dispatchCompute((m_CubeMap->m_Width + 7) / 8, (m_CubeMap->m_Height + 7) / 8, 1);
}

}
