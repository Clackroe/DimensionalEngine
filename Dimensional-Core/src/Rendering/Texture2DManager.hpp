#ifndef TEXTURE2D_MANAGER_HPP
#define TEXTURE2D_MANAGER_HPP

#include "Rendering/OpenGL/OpenGL_Texture2D.hpp"
#include "Rendering/Texture2D.hpp"
namespace Dimensional {

union GraphicsTexture2D {
    OpenGLTexture2D glTexture2D = {};
};

struct Texture2DManager {
    static UUID CreateTexture2D(const Texture2DData& data);

    static void Bind(UUID id, u32 slot);
    static void UnBind(UUID id);

    static void SetData(UUID id, u8* data, u32 width, u32 height);
    static void Resize(UUID id, u32 nWidth, u32 nHeight);

    static void GenerateMipMaps(UUID id);

    static void SetFiletering(UUID id, TextureFiltering tf);
    static void SetWrap(UUID id, TextureWrapMode tf);
    static void SetBorderColor(UUID id, glm::vec4 col);

    static OpenGLTexture2D GetOpenGLTexture(UUID id);

    static void DeleteTexture2D(UUID id);

private:
    static UMap<UUID, GraphicsTexture2D> s_Texture2DMap;
};

}
#endif // TEXTURE2D_MANAGER_HPP
