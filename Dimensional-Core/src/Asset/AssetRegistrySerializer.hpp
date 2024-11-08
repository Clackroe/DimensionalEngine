#ifndef DM_ASSETREG_SERIALIZER_HPP
#define DM_ASSETREG_SERIALIZER_HPP

#include <Asset/AssetManager.hpp>
#include <core.hpp>

namespace Dimensional {
class DMCORE_API AssetRegistrySerializer {
public:
    static void Serialize(std::filesystem::path savePath, AssetManager& manager);
    static void Deserialize(std::filesystem::path file, AssetManager& manager);
};
}

#endif
