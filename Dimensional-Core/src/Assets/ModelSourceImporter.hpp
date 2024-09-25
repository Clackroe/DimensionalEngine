#ifndef DM_MODEL_SOURCE_IMPORTER
#define DM_MODEL_SOURCE_IMPORTER
#include "Assets/AssetMeta.hpp"
#include "Rendering/ModelSource.hpp"
#include <core.hpp>
namespace Dimensional {

class DMCORE_API ModelSourceImporter {
public:
    static Ref<ModelSource> importModelSource(AssetMetaData meta);
    static Ref<ModelSource> loadModelSourceFromPath(std::filesystem::path path);
};
}

#endif
