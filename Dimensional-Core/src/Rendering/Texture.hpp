#ifndef DM_TEXTURE_HPP
#define DM_TEXTURE_HPP

#include "nvrhi/nvrhi.h"
namespace Dimensional {

struct TextureCreateInfo {
    nvrhi::Format format = nvrhi::Format::RGBA8_UNORM;
    u32 width, height;
    u32 depth = 1; // Unused in 2D textures;
    u32 arraySize = 1; // If >1 will be texture array;

    bool generateMipmaps = true;
    bool storage = false;

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

    nvrhi::Format m_Format;

    nvrhi::TextureHandle m_Handle;
    bool m_IsUAV;
};

}

#endif // DM_TEXTURE_HPP
