#include "Assets/ModelImporter.hpp"
#include "Assets/ModelSerializer.hpp"

namespace Dimensional {

Ref<Model> ModelImporter::importModel(AssetMetaData meta)
{
    return loadModelFromPath(meta.sourcePath);
}

Ref<Model> ModelImporter::loadModelFromPath(std::filesystem::path path)
{
    return ModelSerializer::Deserialize(path);
}

}
