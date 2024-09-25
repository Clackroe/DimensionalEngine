#include "Assets/MaterialImporter.hpp"
#include "Assets/MaterialSerializer.hpp"
#include "Rendering/Material.hpp"

namespace Dimensional {

Ref<Material> MaterialImporter::importModel(AssetMetaData meta)
{
    return loadModelFromPath(meta.sourcePath);
}

Ref<Material> MaterialImporter::loadModelFromPath(std::filesystem::path path)
{
    MaterialSettings settings;
    MaterialSerializer::Deserialize(path, settings);

    return CreateRef<Material>(settings);
}

}
