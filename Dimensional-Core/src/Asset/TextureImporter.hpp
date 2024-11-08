#ifndef DM_TEXTURE_IMPORTER
#define DM_TEXTURE_IMPORTER
#include "Asset/AssetMeta.hpp"
#include "Rendering/Texture.hpp"
#include <core.hpp>
namespace Dimensional {

class DMCORE_API TextureImporter {
public:
    static Ref<Texture> importTexture(AssetMetaData meta);
    static Ref<Texture> loadAssetFromPath(std::filesystem::path path, bool hdr = false);
};
}

#endif
