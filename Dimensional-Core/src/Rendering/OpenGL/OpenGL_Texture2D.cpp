
#include "Rendering/OpenGL/OpenGL_Texture2D.hpp"
#include "Rendering/OpenGL/OpenGL_TextureEnums.hpp"
#include <glad.h>

namespace Dimensional {

OpenGLTexture2D OpenGLTexture2D::Create(const Texture2DData& data)
{
    OpenGLTexture2D tex;
    tex.m_Data = data;

    glCreateTextures(GL_TEXTURE_2D, 1, &tex.m_GLID);
    tex.SetWrap(data.wrap);
    tex.SetFiletering(data.filtering);
    tex.Resize(data.width, data.height);

    if (data.data != nullptr) {
        tex.SetData(data.data, data.width, data.height);
    }

    if (data.generateMipMaps && data.data != nullptr) {
        tex.GenerateMipMaps();
    }

    return tex;
}

void OpenGLTexture2D::Bind(u32 slot)
{
    glBindTextureUnit(slot, m_GLID);
}
void OpenGLTexture2D::UnBind()
{
    glBindTextureUnit(0, m_GLID);
}

void OpenGLTexture2D::SetData(u8* data, u32 width, u32 height)
{
    if (width != m_Data.width || height != m_Data.height) {
        Resize(width, height);
    }
    glTextureSubImage2D(m_GLID, 0, 0, 0, width, height, TextureFormatToGLFormat.at(m_Data.format), TextureFormatToGLDataType.at(m_Data.format), data);
}
void OpenGLTexture2D::Resize(u32 nWidth, u32 nHeight)
{
    glTextureStorage2D(m_GLID, 1, TextureFormatToGLInternalFormat.at(m_Data.format), nWidth, nHeight);
}

void OpenGLTexture2D::GenerateMipMaps()
{
    glGenerateTextureMipmap(m_GLID);
}

void OpenGLTexture2D::SetFiletering(TextureFiltering tf)
{
    u32 filter = TextureFilterToGL.at(tf);

    glTextureParameteri(m_GLID, GL_TEXTURE_MIN_FILTER, filter);
    glTextureParameteri(m_GLID, GL_TEXTURE_MAG_FILTER, filter);
}
void OpenGLTexture2D::SetWrap(TextureWrapMode tf)
{
    u32 wrap = TextureWrapToGL.at(tf);
    glTextureParameteri(m_GLID, GL_TEXTURE_WRAP_R, wrap);
    glTextureParameteri(m_GLID, GL_TEXTURE_WRAP_S, wrap);
    glTextureParameteri(m_GLID, GL_TEXTURE_WRAP_T, wrap);
}
void OpenGLTexture2D::SetBorderColor(glm::vec4 col)
{
    float c[] = { col.r, col.g, col.b, col.a };
    glTextureParameterfv(m_GLID, GL_TEXTURE_BORDER_COLOR, c);
}

void OpenGLTexture2D::Destroy()
{
    glDeleteTextures(1, &m_GLID);
}

}
