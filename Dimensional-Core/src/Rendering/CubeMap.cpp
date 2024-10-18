#include "Asset/AssetManager.hpp"
#include "Asset/TextureImporter.hpp"
#include <Rendering/CubeMap.hpp>
#include <Rendering/Renderer.hpp>
#include <glad.h>
namespace Dimensional {
Ref<Shader> CubeMap::s_EquirectToCubeMap = nullptr;

CubeMap::CubeMap(std::string path, u32 w, u32 h)
    : m_Width(w)
    , m_Height(h)
{
    if (!s_EquirectToCubeMap) {
        s_EquirectToCubeMap = CreateRef<Shader>("Assets/Shaders/EquirectToCubeMapComp.glsl", ShaderType::COMPUTE);
    }
    m_EnvironmentTexture = TextureImporter::loadAssetFromPath(path, true);
    createMainTexture();
    renderToCubeMap();
}

void CubeMap::createMainTexture()
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

void CubeMap::renderToCubeMap()
{
    s_EquirectToCubeMap->use();
    m_EnvironmentTexture->bind(0);
    glBindImageTexture(0, m_EnvironmentTexture->getID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGB16F);
    glBindImageTexture(1, m_GLId, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
    s_EquirectToCubeMap->dispatchCompute((m_Width + 15) / 16, (m_Height + 15) / 16, 6);
}

void CubeMap::bind(u32 slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_GLId);
}

}
