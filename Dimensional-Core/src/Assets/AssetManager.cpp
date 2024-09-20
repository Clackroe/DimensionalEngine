#include "Assets/AssetMeta.hpp"
#include <Assets/AssetImporter.hpp>
#include <Assets/AssetManager.hpp>

namespace Dimensional {

static UMap<std::string, AssetType> s_ExtensionToType = {
    { ".png", AssetType::TEXTURE },
    { ".jpg", AssetType::TEXTURE },
    { ".jpeg", AssetType::TEXTURE },
    { ".PNG", AssetType::TEXTURE },
    { ".JPG", AssetType::TEXTURE },
    { ".JPEG", AssetType::TEXTURE },
    { ".obj", AssetType::MODELSOURCE },
    { ".gltf", AssetType::MODELSOURCE },
    { ".fbx", AssetType::MODELSOURCE },
    { ".FBX", AssetType::MODELSOURCE },
    { ".glsl", AssetType::SHADER }
};

Ref<Asset>
AssetManager::getAsset(AssetHandle handle)
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
        if (!outAsset) {
            DM_CORE_WARN("ASSETMANAGER | Asset {0} Load Failed from path {1}", (u64)handle, data.sourcePath);
        }
    }
    return outAsset;
}

AssetHandle AssetManager::registerAsset(std::filesystem::path path)
{
    AssetHandle handle;
    AssetMetaData meta;
    meta.sourcePath = path;
    meta.type = s_ExtensionToType[path.extension()];
    if (meta.type == AssetType::NONE) {
        DM_CORE_WARN("UNABLE TO RECOGNIZE FILE EXTENSION")
    }

    Ref<Asset> asset = AssetImporter::importAsset(meta);
    if (asset) {
        asset->handle = handle;
        m_LoadedAssets[handle] = asset;
        m_Registry[handle] = meta;
        return handle;
        // TODO: Save Asset Registry
    }
    return 0;
}

const AssetMetaData& AssetManager::getMetaData(AssetHandle handle) const
{
    static AssetMetaData s_NullData;
    auto it = m_Registry.find(handle);
    if (it == m_Registry.end()) {
        return s_NullData;
    }
    return it->second;
}

bool AssetManager::isAssetLoaded(AssetHandle handle) { return m_LoadedAssets.find(handle) != m_LoadedAssets.end(); }
bool AssetManager::isAssetRegistered(AssetHandle handle) { return m_Registry.find(handle) != m_Registry.end(); }

}
