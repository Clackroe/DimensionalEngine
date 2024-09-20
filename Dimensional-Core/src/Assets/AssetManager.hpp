#ifndef DM_ASSETMANAGER_H
#define DM_ASSETMANAGER_H

#include "Assets/Asset.hpp"
#include "Assets/AssetMeta.hpp"
#include "Rendering/Shader.hpp"
#include <Core/UUID.hpp>
#include <core.hpp>
namespace Dimensional {

using AssetCache = UMap<AssetHandle, Ref<Asset>>;
using AssetRegistry = UMap<AssetHandle, AssetMetaData>;

class DMCORE_API AssetManager {
public:
    static AssetManager& getInstance()
    {
        static AssetManager manager;
        return manager;
    };

    Ref<Asset> getAsset(AssetHandle handle);
    const AssetMetaData& getMetaData(AssetHandle handle) const;

    AssetHandle registerAsset(std::filesystem::path path);

    bool isAssetLoaded(AssetHandle handle);
    bool isAssetRegistered(AssetHandle handle);
    AssetCache m_LoadedAssets;
    AssetRegistry m_Registry;

private:
    AssetManager() = default;
    ~AssetManager() = default;
};
}

#endif
