#ifndef DM_ASSET_HPP
#define DM_ASSET_HPP
#include "Core/UUID.hpp"
#include <core.hpp>

namespace Dimensional {

enum AssetType {
    TextureType = 0,
    MaterialType = 1,
    ModelType = 2,
    ShaderType = 3,
};

using AssetHandle = UUID;

class DMCORE_API Asset {
public:
    const std::string name;
    const AssetHandle handle;
    const AssetType type;
    const std::string path;

    Asset(const std::string& path, AssetType t)
        : name(generateName(path))
        , type(t)
        , path(path)
    {
    }

private:
    std::string generateName(const std::string& path);
};

}
#endif
