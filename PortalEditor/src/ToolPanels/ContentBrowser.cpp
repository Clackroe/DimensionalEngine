#include "Assets/AssetMeta.hpp"
#include "Assets/ModelSerializer.hpp"
#include "Rendering/Model.hpp"
#include "imgui.h"
#include <Assets/AssetManager.hpp>
#include <ToolPanels/ContentBrowser.hpp>
#include <filesystem>

namespace Dimensional {

static void renderItem(Ref<Texture> texture, std::string& name, BrowserItem item)
{
}

void ContentBrowser::renderImGui()
{
    ImGui::Begin("Content Browser");

    if (ImGui::Button("Refresh")) {
        refreshAssets();
    }
    ImGui::SameLine();

    if (ImGui::Button(m_Mode == BrowsingMode::Asset ? "Asset" : "Filesystem")) {
        if (m_Mode == BrowsingMode::Asset) {
            m_Mode = BrowsingMode::FileSystem;
        } else {
            m_Mode = BrowsingMode::Asset;
        }
    }

    // -----

    static float padding = 12.0f;
    static float iconSize = 128.0f;
    float itemSize = iconSize + padding;

    if (m_CurrentPath != m_RootPath) {
        if (ImGui::Button("../")) {
            m_CurrentPath = m_CurrentPath.parent_path();
        }
    }

    // Get sort the files to ensure that directories are first
    std::vector<std::filesystem::directory_entry> files;
    for (auto& f : std::filesystem::directory_iterator(m_CurrentPath)) {
        if (f.exists())
            files.emplace_back(f.path());
    }
    std::sort(files.begin(), files.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
        if (a.is_directory() != b.is_directory()) {
            return a.is_directory();
        }
        return a.path().filename().string() < b.path().filename().string();
    });

    ImVec2 panelSize = ImGui::GetContentRegionAvail();
    u32 numCols = (u32)(panelSize.x / itemSize);
    numCols = numCols > 0 ? numCols : 1;

    ImGui::Columns(numCols, nullptr, false);
    for (auto& p : files) {

        ImGui::PushID(p.path().string().c_str());

        if (p.is_directory()) {
            if (ImGui::ImageButton((ImTextureID)(uintptr_t)m_FolderIcon->getID(), ImVec2(iconSize, iconSize), { 0, 1 }, { 1, 0 })) {
                m_CurrentPath /= p.path().filename();
                refreshAssets();
            }
        } else {
            if (m_Mode == BrowsingMode::Asset) {
                if (m_Items.contains(p.path().string())) {
                    ImGui::ImageButton((ImTextureID)(uintptr_t)m_FileIcon->getID(), ImVec2(iconSize, iconSize), { 0, 1 }, { 1, 0 });

                    if (ImGui::BeginDragDropSource()) {
                        auto handle = m_Items[p.path().string()].handle;
                        ImGui::SetDragDropPayload("CONTENT_BROWSER_ASSET", &handle, sizeof(handle));
                        ImGui::EndDragDropSource();
                    }
                } else {
                    ImGui::PopID();
                    continue;
                }
            } else {
                ImGui::Image((ImTextureID)(uintptr_t)m_FileIcon->getID(), ImVec2(iconSize, iconSize), { 0, 1 }, { 1, 0 });

                if (ImGui::BeginPopupContextItem("FileContext")) {
                    if (ImGui::MenuItem("Import")) {
                        AssetManager::getInstance().registerAsset(p.path());
                        refreshAssets();
                    }
                    if (m_Items[p.path().string()].type == AssetType::MODELSOURCE) {
                        if (ImGui::MenuItem("Generate Model")) {
                            ModelLoadSettings set;
                            set.modelSource = m_Items[p.path().string()].handle;
                            auto pp = p.path().parent_path() / (p.path().stem().string() + ".dmod");
                            ModelSerializer::Serialize(pp, CreateRef<Model>(set));
                            AssetManager::getInstance().registerAsset(pp);
                            refreshAssets();
                        }
                    }

                    ImGui::EndPopup();
                }
            }
        }
        ImGui::TextWrapped("%s", p.path().filename().string().c_str());
        ImGui::NextColumn();
        ImGui::PopID();
    }
    ImGui::Columns(1);

    ImGui::End();
}

void ContentBrowser::refreshAssets()
{
    m_Items.clear();
    for (auto& [handle, data] : AssetManager::getInstance().m_Registry) {
        m_Items[data.sourcePath] = { handle, data.type };
    }
}

}
