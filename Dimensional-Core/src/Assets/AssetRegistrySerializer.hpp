#ifndef DM_ASSETREG_SERIALIZER_HPP
#define DM_ASSETREG_SERIALIZER_HPP

#include <Assets/AssetManager.hpp>
#include <core.hpp>

namespace Dimensional {
class AssetRegistrySerializer {
public:
    static void Serialize(std::filesystem::path savePath, const AssetManager& manager);
    static void Deserialize(std::filesystem::path file, AssetManager& manager);
};
}

#endif
