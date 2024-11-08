#include "Asset/MaterialImporter.hpp"
#include "Asset/MaterialSerializer.hpp"
#include "Rendering/Material.hpp"

namespace Dimensional {

Ref<Material> MaterialImporter::importMaterial(AssetMetaData meta)
{
    return loadMaterialFromPath(meta.sourcePath);
}

Ref<Material> MaterialImporter::loadMaterialFromPath(std::filesystem::path path)
{
    MaterialSettings settings;
    MaterialSerializer::Deserialize(path, settings);

    return CreateRef<Material>(settings);
}

}
