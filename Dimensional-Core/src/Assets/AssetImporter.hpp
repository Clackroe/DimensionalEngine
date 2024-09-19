#ifndef DM_ASSET_IMPORTER_HPP
#define DM_ASSET_IMPORTER_HPP
#include "Assets/Asset.hpp"
#include <core.hpp>

namespace Dimensional {
class AssetImporter {
public:
    Ref<Asset> importAsset(const AssetMetaData& data);
};
}

#endif
