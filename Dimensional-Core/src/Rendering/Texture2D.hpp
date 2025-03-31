#ifndef TEXTURE2D_AGNOST_HPP
#define TEXTURE2D_AGNOST_HPP

#include "Core/UUID.hpp"
#include <Rendering/TextureEnums.hpp>

namespace Dimensional {

struct Texture2DData {
    u32 width, height, channels;
    u8* data = nullptr;
    TextureFormat format = TextureFormat::DEFAULT;
    TextureWrapMode wrap = TextureWrapMode::DEFAULT;
    TextureFiltering filtering = TextureFiltering::DEFAULT;
    bool generateMipMaps = false;
};

struct Texture2D {

    static Ref<Texture2D> Create(const Texture2DData& data);

    void Bind(u32 slot);
    void UnBind();

    void SetData(u8* data, u32 width, u32 height);
    void Resize(u32 nWidth, u32 uHeight);

    void GenerateMipMaps();

    void SetFiletering(TextureFiltering tf);
    void SetWrap(TextureWrapMode tf);
    void SetBorderColor(glm::vec4 col);

    UUID GetUUID()
    {
        return m_UUID;
    }

private:
    UUID m_UUID;
    Texture2D() = default;
};

};

#endif // TEXTURE2D_AGNOST_HPP
