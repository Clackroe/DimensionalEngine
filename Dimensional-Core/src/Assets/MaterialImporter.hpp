#ifndef DM_MATERIAL_IMPORTER
#define DM_MATERIAL_IMPORTER
#include "Assets/AssetMeta.hpp"
#include "Rendering/Material.hpp"
#include <core.hpp>
namespace Dimensional {

class DMCORE_API MaterialImporter {
public:
    static Ref<Material> importMaterial(AssetMetaData meta);
    static Ref<Material> loadMaterialFromPath(std::filesystem::path path);
};
}

#endif
