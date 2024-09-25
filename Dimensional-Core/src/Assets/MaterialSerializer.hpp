#ifndef DM_MATERIAL_SERIALIZER
#define DM_MATERIAL_SERIALIZER
#include "Rendering/Material.hpp"
#include <Assets/AssetManager.hpp>
#include <Rendering/Model.hpp>
#include <core.hpp>

namespace Dimensional {
class MaterialSerializer {
public:
    static void Serialize(std::filesystem::path savePath, MaterialSettings& material);

    static void Deserialize(std::filesystem::path file, MaterialSettings& settings);
};
}

#endif
