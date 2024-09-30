#include "Assets/AssetMeta.hpp"
#include "Assets/EnvironmentMapImporter.hpp"
#include "Assets/MaterialImporter.hpp"
#include "Assets/ModelImporter.hpp"
#include "Assets/ModelSourceImporter.hpp"
#include "Assets/SceneImporter.hpp"
#include "Assets/TextureImporter.hpp"
#include <Assets/AssetImporter.hpp>
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
