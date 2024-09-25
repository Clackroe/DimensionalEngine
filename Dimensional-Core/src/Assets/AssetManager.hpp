#ifndef DM_ASSETMANAGER_H
#define DM_ASSETMANAGER_H

#include "Assets/Asset.hpp"
#include "Assets/AssetMeta.hpp"
#include "Assets/AssetImporter.hpp"
#include <Core/UUID.hpp>
#include <core.hpp>

class Material;
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
Ref<T> getAsset(AssetHandle handle)
{
    // if it is valid registered asset
    // if not, return invalid
    if (!isAssetRegistered(handle)) {
        return nullptr;
    }
    // Check if it needs load
    Ref<Asset> outAsset = nullptr;
    if (isAssetLoaded(handle)) {
        outAsset = m_LoadedAssets.at(handle);
    } else {
        // load/return i
        const AssetMetaData& data = getMetaData(handle);
        outAsset = AssetImporter::importAsset(data);
        outAsset->handle = m_PathToHandle[data.sourcePath];
        if (!outAsset) {
            DM_CORE_WARN("ASSETMANAGER | Asset {0} Load Failed from path {1}", (u64)handle, data.sourcePath);
            return nullptr;
        }
        m_LoadedAssets[handle] = outAsset;
    }
    auto castedAsset = std::dynamic_pointer_cast<T>(outAsset);
    if (!castedAsset) {
        DM_CORE_WARN("ASSETMANAGER | Attempted to cast Asset {0}, of type {1}, to the wrong type.", (u64)handle, Asset::assetTypeToString(m_Registry[handle].type));
        return nullptr;
    }

    return castedAsset;
}


    const AssetMetaData& getMetaData(AssetHandle handle) const;

    std::vector<AssetHandle> getAssetHandles(AssetType type);

    AssetHandle getAssetHandleFromPath(std::string path);

    AssetHandle registerAsset(std::filesystem::path path);

    bool isAssetLoaded(AssetHandle handle);
    bool isAssetRegistered(AssetHandle handle);
    bool isAssetRegistered(std::string path);

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
