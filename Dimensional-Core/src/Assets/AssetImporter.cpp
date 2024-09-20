
#include "Assets/AssetMeta.hpp"
#include "Assets/ModelImporter.hpp"
#include "Assets/TextureImporter.hpp"
#include <Assets/AssetImporter.hpp>
namespace Dimensional {

Ref<Asset> AssetImporter::importAsset(const AssetMetaData& data)
{
    Ref<Asset> out;
    switch (data.type) {
    case (AssetType::TEXTURE):
        out = TextureImporter::importTexture(data);
    case (AssetType::MODEL):
        out = ModelImporter::importModel(data);
    default:
        DM_CORE_ASSERT(false, "Attempted to import texture of unimplemented type");
    }
    return out;
}
}
