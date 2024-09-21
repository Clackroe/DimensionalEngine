#ifndef DM_CONTENTBROWSER_HPP
#define DM_CONTENTBROWSER_HPP

#include "Assets/Asset.hpp"
#include "Assets/AssetMeta.hpp"
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
        refreshAssets();
    }

    void renderImGui();

private:
    std::filesystem::path m_RootPath;
    std::filesystem::path m_CurrentPath;

    UMap<std::string, BrowserItem> m_Items;

    BrowsingMode m_Mode;

    void refreshAssets();
};
}

#endif
