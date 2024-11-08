#ifndef DM_ASSET_IMPORTER_HPP
#define DM_ASSET_IMPORTER_HPP
#include "Asset/Asset.hpp"
#include <core.hpp>

namespace Dimensional {
class DMCORE_API AssetImporter {
public:
    static Ref<Asset> importAsset(const AssetMetaData& data);
};
}

#endif
