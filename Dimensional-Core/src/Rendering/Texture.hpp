#ifndef DM_TEXTURE_H
#define DM_TEXTURE_H
#include <Assets/Asset.hpp>
#include <core.hpp>

namespace Dimensional {

enum class ImageFormat {
    NONE = 0,
    R8,
    R16,
    R32,
    RGB8,
    RGB16,
    RGB32,
    RGBA8,
    RGBA16,
    RGBA32
};

struct TextureLoadSettings {
    u32 width, height, channels;
    ImageFormat format = ImageFormat::RGBA8;
    bool generateMipmaps = true;
};

class DMCORE_API Texture : public Asset {
public:
    Texture(TextureLoadSettings settings, void* data, u32 sizeBytes);
    ~Texture();

    void bind(u32 textureSlot);

    void setData(void* data, u32 sizeBytes);
    u32 getID() const { return m_GLId; };

    virtual AssetType getAssetType() const { return AssetType::TEXTURE; }

private:
    void load(void* data, u32 sizeBytes);
    u32 m_GLId;

    u32 m_InternalDataFormat, m_DataFormat;
    TextureLoadSettings m_Settings;
};
}

#endif
