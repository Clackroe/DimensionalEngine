#include <Core/Texture.hpp>

#include <glad.h>
#include <stb_image.hpp>

namespace Dimensional {
Texture::Texture(std::string path)
    : Asset(path)
{
    m_Path = path;
};

void Texture::load(std::string path)
{

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //

    int w, h, c;
    unsigned char* data = stbi_load(m_Path.c_str(), &w, &h, &c, 0);
    if (data) {
        m_Width = w;
        m_Height = h;
        m_Channels = c;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_GLId);

        glTextureStorage2D();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        DM_CORE_ERROR("Failed to load texture");
    }
    stbi_image_free(data);
}
void Texture::bind(u32 textureSlot)
{
}

}
