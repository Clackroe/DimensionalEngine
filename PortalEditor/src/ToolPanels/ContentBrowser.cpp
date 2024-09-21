#include "imgui.h"
#include <Assets/AssetManager.hpp>
#include <ToolPanels/ContentBrowser.hpp>
#include <filesystem>

namespace Dimensional {

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

    ImGui::BeginListBox("Test");
    if (m_CurrentPath != m_RootPath) {
        if (ImGui::Button("../")) {
            m_CurrentPath = m_CurrentPath.parent_path();
        }
    }
    for (auto& p : std::filesystem::directory_iterator(m_CurrentPath)) {

        ImGui::PushID(p.path().c_str());

        if (p.is_directory()) {
            if (ImGui::Button(p.path().string().c_str())) {
                m_CurrentPath /= p.path().filename();
            }
            continue;
        }
        if (m_Mode == BrowsingMode::Asset) {
            if (m_Items.contains(p.path().string())) {
                ImGui::Text("%s", p.path().filename().c_str());
            }
        } else {
            ImGui::Text("%s", p.path().filename().c_str());
            if (ImGui::BeginPopupContextItem("FileContext")) {
                if (ImGui::MenuItem("Import")) {
                    AssetManager::getInstance().registerAsset(p.path());
                    refreshAssets();
                }

                ImGui::EndPopup();
            }
        }
        ImGui::PopID();
    }
    ImGui::EndListBox();

    ImGui::EndChild();

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
