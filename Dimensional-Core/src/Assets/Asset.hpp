#ifndef DM_ASSET_HPP
#define DM_ASSET_HPP
#include "Core/UUID.hpp"
#include <Assets/AssetMeta.hpp>
#include <core.hpp>

namespace Dimensional {

using AssetHandle = UUID;

class DMCORE_API Asset {
public:
    const AssetHandle handle;

    Asset()
        : handle(UUID())
    {
    }

    ~Asset() = default;

    AssetType getType() const;
    const std::string& getSourcePath() const;

private:
};

}
#endif
