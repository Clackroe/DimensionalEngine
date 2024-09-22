#ifndef DM_ASSETMANAGER_H
#define DM_ASSETMANAGER_H

#include "Assets/Asset.hpp"
#include "Assets/AssetMeta.hpp"
#include <Core/UUID.hpp>
#include <core.hpp>
namespace Dimensional {

class AssetRegistrySerializer;

using AssetCache = UMap<AssetHandle, Ref<Asset>>;
using AssetRegistry = std::map<AssetHandle, AssetMetaData>;

class DMCORE_API AssetManager {
public:
    static AssetManager& getInstance()
    {
        static AssetManager manager;
        return manager;
    };

    template <typename T>
    Ref<T> getAsset(AssetHandle handle);
    const AssetMetaData& getMetaData(AssetHandle handle) const;

    AssetHandle registerAsset(std::filesystem::path path);

    bool isAssetLoaded(AssetHandle handle);
    bool isAssetRegistered(AssetHandle handle);

    void refresh();

private:
    AssetManager() = default;
    ~AssetManager() = default;

    AssetCache m_LoadedAssets;
    AssetRegistry m_Registry;

    UMap<std::string, AssetHandle> m_PathToHandle;

    //
    friend class PortalLayer;
    //

    friend class ContentBrowser;
    friend class AssetRegistrySerializer;
};
}

#endif
