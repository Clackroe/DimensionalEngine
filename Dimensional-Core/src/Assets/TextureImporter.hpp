#ifndef DM_TEXTURE_IMPORTER
#define DM_TEXTURE_IMPORTER
#include "Assets/AssetMeta.hpp"
#include "Rendering/Texture.hpp"
#include <core.hpp>
namespace Dimensional {

class TextureImporter {
public:
    static Ref<Texture> importTexture(AssetMetaData meta);
    static Ref<Texture> loadAssetFromPath(std::filesystem::path path);
};
}

#endif
