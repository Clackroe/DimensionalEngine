#include "Asset/AssetMeta.hpp"
#include "Asset/EnvironmentMapImporter.hpp"
#include "Asset/MaterialImporter.hpp"
#include "Asset/ModelImporter.hpp"
#include "Asset/ModelSourceImporter.hpp"
#include "Asset/SceneImporter.hpp"
#include "Asset/TextureImporter.hpp"
#include <Asset/AssetImporter.hpp>
#include <filesystem>
namespace Dimensional {

Ref<Asset> AssetImporter::importAsset(const AssetMetaData& data)
{
    if (!std::filesystem::exists(data.sourcePath)) {
        return nullptr;
    }
    Ref<Asset> out = nullptr;
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
        out = MaterialImporter::importMaterial(data);
        break;
    case (AssetType::ENVIRONMENTMAP):
        out = EnvironmentMapImporter::importEnvironment(data);
        break;
    case (AssetType::SCENE):
        out = SceneImporter::importScene(data);
        break;
    default:
        DM_CORE_WARN("Attempted to import texture of unimplemented type");
        break;
    }
    return out;
}
}
