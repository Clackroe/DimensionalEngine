#ifndef DM_MODEL_IMPORTER
#define DM_MODEL_IMPORTER
#include "Asset/AssetMeta.hpp"
#include "Rendering/Model.hpp"
#include <core.hpp>
namespace Dimensional {

class DMCORE_API ModelImporter {
public:
    static Ref<Model> importModel(AssetMetaData meta);
    static Ref<Model> loadModelFromPath(std::filesystem::path path);
};
}

#endif
