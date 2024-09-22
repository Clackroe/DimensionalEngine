#include "Assets/AssetMeta.hpp"
#include "Assets/MaterialImporter.hpp"
#include "Assets/ModelImporter.hpp"
#include "Assets/ModelSourceImporter.hpp"
#include "Assets/TextureImporter.hpp"
#include <Assets/AssetImporter.hpp>
namespace Dimensional {

Ref<Asset> AssetImporter::importAsset(const AssetMetaData& data)
{
    Ref<Asset> out;
    switch (data.type) {
    case (AssetType::TEXTURE):
        out = TextureImporter::importTexture(data);
        break;
    case (AssetType::MODELSOURCE):
        out = ModelSourceImporter::importModelSource(data);
        break;
    case (AssetType::MODEL):
        out = ModelImporter::importModel(data);
        break;
    case (AssetType::MATERIAL):
        out = MaterialImporter::importModel(data);
        break;

    default:
        DM_CORE_WARN("Attempted to import texture of unimplemented type");
        break;
    }
    return out;
}
}
