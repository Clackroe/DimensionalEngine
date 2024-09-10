#include "Log/log.hpp"
#include "core.hpp"
#include <Rendering/Texture.hpp>

#include <glad.h>
#include <stb_image.hpp>

namespace Dimensional {
Texture::Texture(std::string path, bool retainInMemory)
    : Asset(path, AssetType::TextureType)
{
    m_Path = path;
    load(path, retainInMemory);
};
Texture::Texture(u32 width, u32 height)
    : Asset("Blank Texture", AssetType::TextureType)
    , m_Width(width)
    , m_Height(height)
{
    m_IntFormat = GL_RGBA8;
    m_DataFormat = GL_RGBA;

    glCreateTextures(GL_TEXTURE_2D, 1, &m_GLId);
    glTextureStorage2D(m_GLId, 1, m_IntFormat, m_Width, m_Height);

    glTextureParameteri(m_GLId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_GLId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureParameteri(m_GLId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_GLId, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::load(std::string path, bool retainInMemory, bool hdr)
{
    stbi_set_flip_vertically_on_load(1);

    int w, h, c;
    u8* data = stbi_load(m_Path.c_str(), &w, &h, &c, 0);
    if (!data) {
        DM_CORE_WARN("Failed to load texture");
    }

    m_Width = w;
    m_Height = h;
    m_Channels = c;

    // Internal and data Formats to notify opengl
    GLenum intFormat = 0, dataFormat = 0;

    DM_CORE_INFO("Name: {0} Channels: {1}", name, m_Channels);
    switch (m_Channels) {
    case 1:

        intFormat = hdr ? GL_R16F : GL_R8;
        dataFormat = GL_RED;
        break;
    case 3:
        intFormat = hdr ? GL_RGB16F : GL_RGB8;
        dataFormat = GL_RGB;
        break;
    case 4:
        intFormat = hdr ? GL_RGBA16F : GL_RGBA8;
        dataFormat = GL_RGBA;
        break;
    default:
        DM_CORE_ASSERT(false, "Texture (" + name + ")uses an unsupported channel." + std::to_string(m_Channels));
        stbi_image_free(data);
        return;
    }
    m_IntFormat = intFormat;
    m_DataFormat = dataFormat;

    glCreateTextures(GL_TEXTURE_2D, 1, &m_GLId);
    glTextureStorage2D(m_GLId, 1, m_IntFormat, m_Width, m_Height);

    //
    glTextureParameteri(m_GLId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_GLId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_GLId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_GLId, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //

    glTextureSubImage2D(m_GLId, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

    if (retainInMemory) {
        m_TextureData.resize(m_Height * m_Width * m_Channels);
        std::memcpy(m_TextureData.data(), data, m_Height * m_Width * m_Channels * sizeof(u8));
    } else {
        stbi_image_free(data);
    }
}
void Texture::setData(void* data, u32 sizeBytes)
{
    DM_CORE_ASSERT(sizeBytes == m_Width * m_Height * (m_DataFormat == GL_RGBA ? 4 : 3), "Data must be entire texture!")
    glTextureSubImage2D(m_GLId, 0, 0, 0, static_cast<int>(m_Width), static_cast<int>(m_Height), m_DataFormat, GL_UNSIGNED_BYTE, data);
}

void Texture::bind(u32 textureSlot)
{
    glBindTextureUnit(textureSlot, m_GLId);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_GLId);
}

}
