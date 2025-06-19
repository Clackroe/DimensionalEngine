#ifndef DM_TEXTURE_HPP
#define DM_TEXTURE_HPP

#include "nvrhi/nvrhi.h"
namespace Dimensional {

enum class TextureFormat {
    NONE,
    R,
    RG,
    RGB,
    RGBA,

    // DEPTH
    Depth16,
    Depth24,
    Depth32F,
    Depth24Stencil8,
};

inline nvrhi::Format TexFormatToNVRI(TextureFormat format, bool hdr = false)
{
    switch (format) {
    case TextureFormat::R:
        return hdr ? nvrhi::Format::R32_FLOAT : nvrhi::Format::R8_UNORM;
    case TextureFormat::RG:
        return hdr ? nvrhi::Format::RG32_FLOAT : nvrhi::Format::RG8_UNORM;
    case TextureFormat::RGB:
        return hdr ? nvrhi::Format::RGB32_FLOAT : nvrhi::Format::RGBA8_UNORM;
    case TextureFormat::RGBA:
        return hdr ? nvrhi::Format::RGBA32_FLOAT : nvrhi::Format::RGBA8_UNORM;
    case TextureFormat::Depth16:
        return nvrhi::Format::D16;
    case TextureFormat::Depth24:
        return nvrhi::Format::D24S8;
    case TextureFormat::Depth32F:
        return nvrhi::Format::D32;
    case TextureFormat::Depth24Stencil8:
        return nvrhi::Format::D24S8;
        break;
    case TextureFormat::NONE:
        return nvrhi::Format::UNKNOWN;

        break;
    }
}

struct TextureCreateInfo {
    TextureFormat format = TextureFormat::RGBA;
    bool isHDR = false;

    u32 width, height;
    u32 depth = 1; // Unused in 2D textures;
    u32 arraySize = 1; // If >1 will be texture array;

    u32 sampleCount = 1; // MSAA

    bool generateMipmaps = true;
    bool storage = false;
    bool isRenderTarget;

    std::string debugName = "Unamed Texture";
};

class Texture2D {

public:
    static Ref<Texture2D> Create(const TextureCreateInfo& info);

    void SetData(const unsigned char* data, size_t sizeBytes, u32 mip = 0);
    void Resize(u32 width, u32 height);

    nvrhi::TextureHandle GetHandle() const { return m_Handle; }
    u32 GetWidth() const { return m_Width; }
    u32 GetHeight() const { return m_Height; }

    bool IsStorage() const { return m_IsUAV; }

private:
    Texture2D() = default;

    bool Init(nvrhi::DeviceHandle dev, const TextureCreateInfo& info);

    u32 m_Width, m_Height, m_Depth, m_ArraySize;

    bool m_GenerateMips = false;

    std::string m_DebugName;

    TextureFormat m_Format;

    nvrhi::TextureHandle m_Handle;
    bool m_IsUAV;
};

}

#endif // DM_TEXTURE_HPP
