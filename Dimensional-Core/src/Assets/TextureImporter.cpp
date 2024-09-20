#include "Log/log.hpp"
#include "Rendering/Texture.hpp"
#include <Assets/TextureImporter.hpp>
#include <stb_image.hpp>

namespace Dimensional {

Ref<Texture> TextureImporter::importTexture(AssetMetaData meta)
{
    return loadAssetFromPath(meta.sourcePath);
}
Ref<Texture> TextureImporter::loadAssetFromPath(std::filesystem::path path)
{
    stbi_set_flip_vertically_on_load(1);

    int w, h, c;
    u8* data = stbi_load(path.c_str(), &w, &h, &c, 0);
    if (!data) {
        DM_CORE_WARN("Failed to load texture");
        return nullptr;
    }

    ImageFormat format;
    switch (c) {
    case (1):
        format = ImageFormat::R8;
        break;
    case (3):
        format = ImageFormat::RGB8;
        break;
    case (4):
        format = ImageFormat::RGBA8;
        break;
    }

    TextureLoadSettings loadSettings;
    loadSettings.width = w;
    loadSettings.height = h;
    loadSettings.channels = c;
    loadSettings.format = format;

    loadSettings.generateMipmaps = true;

    u32 size = w * h * c;
    Ref<Texture> out = CreateRef<Texture>(loadSettings, data, size * sizeof(u8));
    stbi_image_free(data);

    return out;
}

}
