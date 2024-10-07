#include "Scene/SceneSerializer.hpp"
#include <Assets/SceneImporter.hpp>
namespace Dimensional {

Ref<Scene> SceneImporter::importScene(AssetMetaData meta)
{
    return loadSceneFromPath(meta.sourcePath);
}
Ref<Scene> SceneImporter::loadSceneFromPath(std::filesystem::path path)
{
    Ref<Scene> scene = CreateRef<Scene>();
    SceneSerializer::Deserialize(path, scene);
    return scene;
}
}
