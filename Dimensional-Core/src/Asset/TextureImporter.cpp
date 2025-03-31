// #include "Log/log.hpp"
// #include "Rendering/Texture.hpp"
// #include <Asset/TextureImporter.hpp>
// #include <stb_image.hpp>
//
// namespace Dimensional {
//
// Ref<Texture> TextureImporter::importTexture(AssetMetaData meta)
// {
//     return loadAssetFromPath(meta.sourcePath);
// }
// Ref<Texture> TextureImporter::loadAssetFromPath(std::filesystem::path path, bool hdr)
// {
//     stbi_set_flip_vertically_on_load(1);
//
//     int w, h, c;
//     u8* data = stbi_load(path.string().c_str(), &w, &h, &c, 0);
//     if (!data) {
//         DM_CORE_WARN("Failed to load texture {}", path.string());
//         static Ref<Texture> s_WhiteTexture;
//
//         // temp
//         if (!s_WhiteTexture) {
//             u32 data = 0x80808080;
//             TextureLoadSettings t;
//             t.width = 1;
//             t.height = 1;
//             t.channels = 4;
//             s_WhiteTexture = CreateRef<Texture>(t, &data, sizeof(u32));
//         }
//
//         return s_WhiteTexture;
//     }
//
//     ImageFormat format;
//     switch (c) {
//     case (1):
//         format = hdr ? ImageFormat::R32 : ImageFormat::R8;
//         break;
//     case (3):
//         format = hdr ? ImageFormat::RGB32 : ImageFormat::RGB8;
//         break;
//     case (4):
//         format = hdr ? ImageFormat::RGBA32 : ImageFormat::RGBA8;
//         break;
//     }
//
//     TextureLoadSettings loadSettings;
//     loadSettings.width = w;
//     loadSettings.height = h;
//     loadSettings.channels = c;
//     loadSettings.format = format;
//
//     loadSettings.generateMipmaps = true;
//
//     u32 size = w * h * c;
//     Ref<Texture> out = CreateRef<Texture>(loadSettings, data, size * sizeof(u8));
//     stbi_image_free(data);
//
//     return out;
// }
//
// }
