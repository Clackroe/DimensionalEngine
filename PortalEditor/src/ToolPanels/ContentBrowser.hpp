#ifndef DM_CONTENTBROWSER_HPP
#define DM_CONTENTBROWSER_HPP

#include "Asset/Asset.hpp"
#include "Asset/AssetMeta.hpp"
#include "Asset/TextureImporter.hpp"
#include <core.hpp>
#include <imgui.h>

namespace Dimensional {

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
        // m_FileIcon = TextureImporter::loadAssetFromPath("Assets/Resources/File.png");
        // m_FolderIcon = TextureImporter::loadAssetFromPath("Assets/Resources/Folder.png");
    }

    void renderImGui();

private:
    std::filesystem::path m_RootPath;
    std::filesystem::path m_CurrentPath;

    // Ref<Texture> m_FileIcon;
    // Ref<Texture> m_FolderIcon;
};
}

#endif
