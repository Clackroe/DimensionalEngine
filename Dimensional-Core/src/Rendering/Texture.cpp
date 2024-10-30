#include "core.hpp"
#include <Rendering/Texture.hpp>

#include <glad.h>
#include <stb_image.hpp>

namespace Dimensional {

u32 Texture::imageFormatToInternalFormat(ImageFormat format)
{
    switch (format) {
    case (ImageFormat::R8):
        return GL_R8;
        break;
    case (ImageFormat::R16):
        return GL_R16F;
        break;
    case (ImageFormat::R32):
        return GL_R32F;
        break;
    case (ImageFormat::RGB8):
        return GL_RGB8;
        break;
    case (ImageFormat::RGB16):
        return GL_RGB16F;
        break;
    case (ImageFormat::RGB32):
        return GL_RGB32F;
        break;
    case (ImageFormat::RGBA8):
        return GL_RGBA8;
        break;
    case (ImageFormat::RGBA16):
        return GL_RGBA16F;
        break;
    case (ImageFormat::RGBA32):
        return GL_RGBA32F;
        break;
    case (ImageFormat::DEPTH32F):
        return GL_DEPTH_COMPONENT32F;
        break;
    default:
        DM_CORE_ASSERT(false, "Tried to get an image with NONE format type");
    }
    return 0;
}

u32 Texture::imageFormatToDataFormat(ImageFormat format)
{
    switch (format) {
    case (ImageFormat::R8):
    case (ImageFormat::R16):
    case (ImageFormat::R32):
        return GL_RED;
        break;
    case (ImageFormat::RGB8):
    case (ImageFormat::RGB16):
    case (ImageFormat::RGB32):
        return GL_RGB;
        break;
    case (ImageFormat::RGBA8):
    case (ImageFormat::RGBA16):
    case (ImageFormat::RGBA32):
        return GL_RGBA;
        break;
    default:
        DM_CORE_ASSERT(false, "Tried to get an image with NONE format type");
    }
    return 0;
}

Texture::Texture(TextureLoadSettings settings, void* data, u32 sizeBytes)
{
    m_Settings = settings;
    load(data, sizeBytes);
}

void Texture::load(void* data, u32 sizeBytes)
{

    m_InternalDataFormat = imageFormatToInternalFormat(m_Settings.format);
    m_DataFormat = imageFormatToDataFormat(m_Settings.format);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_GLId);
    glTextureStorage2D(m_GLId, 1, m_InternalDataFormat, m_Settings.width, m_Settings.height);

    // TODO: Make These A Parameter to be Chosen
    glTextureParameteri(m_GLId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_GLId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_GLId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_GLId, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //

    setData(data, sizeBytes);
}
void Texture::setData(void* data, u32 sizeBytes)
{
    DM_CORE_ASSERT(sizeBytes == m_Settings.width * m_Settings.height * m_Settings.channels, "Data must be entire texture!")
    glTextureSubImage2D(m_GLId, 0, 0, 0, m_Settings.width, m_Settings.height, m_DataFormat, GL_UNSIGNED_BYTE, data);
}

void Texture::bind(u32 textureSlot)
{
    glBindTextureUnit(textureSlot, m_GLId);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_GLId);
}

// =========== TEXTURE VIEW ============

TextureView::TextureView(u32 textureArray, ImageFormat format, u32 layerIndex)
{
    glGenTextures(1, &glID);
    glTextureView(glID, GL_TEXTURE_2D, textureArray, GL_R32F, 0, 1, layerIndex, 1);
}
TextureView::~TextureView()
{
    glDeleteTextures(1, &glID);
}

}
