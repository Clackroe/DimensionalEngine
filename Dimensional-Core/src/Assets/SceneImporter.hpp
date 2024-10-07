#ifndef DM_SCENE_IMPORTER
#define DM_SCENE_IMPORTER
#include "Scene/Scene.hpp"
#include <Assets/AssetManager.hpp>
#include <core.hpp>

namespace Dimensional {
class DMCORE_API SceneImporter {
public:
    static Ref<Scene> importScene(AssetMetaData meta);
    static Ref<Scene> loadSceneFromPath(std::filesystem::path path);
};
}

#endif
