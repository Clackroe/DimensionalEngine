#include "Assets/AssetMeta.hpp"
#include "Log/log.hpp"
#include "Rendering/EnvironmentMap.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/Texture.hpp"
#include <Assets/AssetImporter.hpp>
#include <Assets/AssetManager.hpp>
#include <Assets/AssetRegistrySerializer.hpp>
#include <memory>

namespace Dimensional {

static UMap<std::string, AssetType> s_ExtensionToType = {
    { ".png", AssetType::TEXTURE },
    { ".jpg", AssetType::TEXTURE },
    { ".jpeg", AssetType::TEXTURE },
    { ".PNG", AssetType::TEXTURE },
    { ".JPG", AssetType::TEXTURE },
    { ".JPEG", AssetType::TEXTURE },
    { ".tga", AssetType::TEXTURE },
    { ".obj", AssetType::MODELSOURCE },
    { ".gltf", AssetType::MODELSOURCE },
    { ".fbx", AssetType::MODELSOURCE },
    { ".FBX", AssetType::MODELSOURCE },
    { ".dmod", AssetType::MODEL },
    { ".glsl", AssetType::SHADER },
    { ".dmat", AssetType::MATERIAL },
    { ".hdr", AssetType::ENVIRONMENTMAP }
};

// Notify compiler of use outside of lib
// template Ref<Texture> AssetManager::getAsset<Texture>(AssetHandle handle);
// template Ref<Model> AssetManager::getAsset<Model>(AssetHandle handle);
// template Ref<ModelSource> AssetManager::getAsset<ModelSource>(AssetHandle handle);
// template Ref<Material> AssetManager::getAsset<Material>(AssetHandle handle);
// template Ref<EnvironmentMap> AssetManager::getAsset<EnvironmentMap>(AssetHandle handle);
// template Ref<Shader> AssetManager::getAsset<Shader>(AssetHandle handle);

AssetHandle AssetManager::registerAsset(std::filesystem::path path)
{
    if (m_PathToHandle.contains(path.string())) {
        AssetHandle handle = m_PathToHandle[path.string()];
        return handle;
    }

    AssetHandle handle;
    AssetMetaData meta;
    meta.sourcePath = path.string();
    auto it = s_ExtensionToType.find(path.extension().string());
    if (it != s_ExtensionToType.end() && it->second != AssetType::NONE) {
        auto t = it->second;
        meta.type = t;
    } else {
        DM_CORE_WARN("UNABLE TO RECOGNIZE FILE EXTENSION \"{}\"", path.extension().string())
        return 0;
    }
    m_Registry[handle] = meta;
    AssetRegistrySerializer::Serialize("Assets/Registry.dreg", AssetManager::getInstance());
    return handle;
}

std::vector<AssetHandle> AssetManager::getAssetHandles(AssetType type)
{
    std::vector<AssetHandle> out;
    for (auto [handle, meta] : m_Registry) {
        if (meta.type == type) {
            out.emplace_back(handle);
        }
    }
    return out;
}

const AssetMetaData&
AssetManager::getMetaData(AssetHandle handle) const
{
    static AssetMetaData s_NullData;
    auto it = m_Registry.find(handle);
    if (it == m_Registry.end()) {
        return s_NullData;
    }
    return it->second;
}

bool AssetManager::isAssetLoaded(AssetHandle handle)
{
    return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
}
bool AssetManager::isAssetRegistered(AssetHandle handle)
{
    return m_Registry.find(handle) != m_Registry.end();
}

AssetHandle AssetManager::getAssetHandleFromPath(std::string path)
{

    auto it = m_PathToHandle.find(path);
    if (it != m_PathToHandle.end()) {
        return it->second;
    }
    return 0;
}
bool AssetManager::isAssetRegistered(std::string path)
{
    auto it = m_PathToHandle.find(path);
    if (it == m_PathToHandle.end()) {
        return false;
    }
    return m_Registry.find(it->second) != m_Registry.end();
}

void AssetManager::refresh()
{
    m_PathToHandle.clear();
    for (auto [handle, meta] : m_Registry) {
        m_PathToHandle[meta.sourcePath] = handle;
    }
};

}
