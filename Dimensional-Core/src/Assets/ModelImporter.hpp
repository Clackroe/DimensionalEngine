#ifndef DM_TEXTURE_IMPORTER
#define DM_TEXTURE_IMPORTER
#include "Assets/AssetMeta.hpp"
#include "Rendering/Model.hpp"
#include <core.hpp>
namespace Dimensional {

class ModelImporter {
public:
    static Ref<Model> importModel(AssetMetaData meta);
    static Ref<Model> loadModelFromPath(std::filesystem::path path);
};
}

#endif
