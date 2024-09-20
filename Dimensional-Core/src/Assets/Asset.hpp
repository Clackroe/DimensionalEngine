#ifndef DM_ASSET_HPP
#define DM_ASSET_HPP
#include "Core/UUID.hpp"
#include <Assets/AssetMeta.hpp>
#include <core.hpp>

namespace Dimensional {

using AssetHandle = UUID;

class DMCORE_API Asset {
public:
    AssetHandle handle;

    virtual AssetType getAssetType() const = 0;
};

}
#endif
