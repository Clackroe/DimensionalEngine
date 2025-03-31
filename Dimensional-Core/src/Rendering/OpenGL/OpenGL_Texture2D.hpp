#ifndef OPENGL_TEXTURE2D
#define OPENGL_TEXTURE2D
#include "Rendering/Texture2D.hpp"
namespace Dimensional {

struct OpenGLTexture2D {

    static OpenGLTexture2D Create(const Texture2DData& data);

    void Bind(u32 slot);
    void UnBind();
    void SetData(u8* data, u32 width, u32 height);
    void Resize(u32 nWidth, u32 uHeight);

    void GenerateMipMaps();

    void SetFiletering(TextureFiltering tf);
    void SetWrap(TextureWrapMode tf);

    void Destroy();

private:
    u32 m_GLID;
    Texture2DData m_Data;
};

}

#endif // OPENGL_TEXTURE2D
