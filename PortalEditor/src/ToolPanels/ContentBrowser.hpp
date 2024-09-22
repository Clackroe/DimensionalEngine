#ifndef DM_CONTENTBROWSER_HPP
#define DM_CONTENTBROWSER_HPP

#include "Assets/Asset.hpp"
#include "Assets/AssetMeta.hpp"
#include "Assets/TextureImporter.hpp"
#include "Rendering/Texture.hpp"
#include <core.hpp>
#include <imgui.h>

namespace Dimensional {

enum class BrowsingMode {
    Asset = 0,
    FileSystem = 1
};

struct BrowserItem {
    AssetHandle handle;
    AssetType type;
};

class ContentBrowser {
public:
    ContentBrowser(const std::string& rootPath)
        : m_RootPath(rootPath)
        , m_CurrentPath(rootPath)
    {
        m_FileIcon = TextureImporter::loadAssetFromPath("Assets/Resources/File.png");
        m_FolderIcon = TextureImporter::loadAssetFromPath("Assets/Resources/Folder.png");
        refreshAssets();
    }

    void renderImGui();

private:
    std::filesystem::path m_RootPath;
    std::filesystem::path m_CurrentPath;

    UMap<std::string, BrowserItem> m_Items;

    BrowsingMode m_Mode;
    Ref<Texture> m_FileIcon;
    Ref<Texture> m_FolderIcon;

    void refreshAssets();
};
}

#endif