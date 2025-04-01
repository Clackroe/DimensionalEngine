#include "Asset/AssetMeta.hpp"
// #include "Rendering/Model.hpp"
#include "imgui.h"
#include <Asset/AssetManager.hpp>
#include <Asset/ModelSerializer.hpp>
#include <ToolPanels/ContentBrowser.hpp>
#include <filesystem>

namespace Dimensional {

// TEMPORARY
static std::string DefaultMaterial = R"(Material:
  Albedo: 0
  Normal: 0
  Metal: 0
  Roughness: 0
  AO: 0
  UseMetalMap: false
  UseRoughnessMap: false
  Color: [1.0, 1.0, 1.0]
  MetalnessMultiplier: 1
  RoughnessMultiplier: 1
)";

static std::string DefaultScene = "Scene: Untitled Scene";

// TODO: Move to a filesystem class/namespace
namespace Utils {
    static void newFile(std::filesystem::path path, std::string content)
    {
        std::filesystem::path parent = path.parent_path();
        std::string baseName = path.stem().string();
        std::filesystem::path ext = path.extension();
        u32 num = 1;

        while (std::filesystem::exists(path)) {
            std::string newName = baseName + std::to_string(num);
            std::filesystem::path fileName = std::filesystem::path(newName + ext.string());
            path = parent / fileName;
            ++num;
        }
        std::ofstream file(path);
        if (file) {
            file << content;
            file.close();
        } else {
            DM_CORE_WARN("Failed to create file {}", path.string());
        }
    }
}

static std::vector<std::filesystem::directory_entry> getSortedDirectory(std::filesystem::path path)
{
    // Get sort the files to ensure that directories are first
    std::vector<std::filesystem::directory_entry> files;
    for (auto& f : std::filesystem::directory_iterator(path)) {
        if (f.exists())
            files.emplace_back(f.path());
    }
    std::sort(files.begin(), files.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
        if (a.is_directory() != b.is_directory()) {
            return a.is_directory();
        }
        return a.path().filename().string() < b.path().filename().string();
    });

    return files;
}

// static void renderItem(Ref<Texture> texture, glm::vec2 size, const std::string& name, BrowserItem item)
// {
//     ImGui::ImageButton((ImTextureID)(uintptr_t)texture->getID(), ImVec2(size.x, size.y), { 0, 1 }, { 1, 0 });
//     if (ImGui::BeginDragDropSource()) {
//         ImGui::SetDragDropPayload("CONTENT_BROWSER_ASSET", &item.handle, sizeof(item.handle));
//         ImGui::EndDragDropSource();
//     }
// }

void ContentBrowser::renderImGui()
{
    ImGui::Begin("Content Browser");

    static float padding = 12.0f;
    static float iconSize = 128.0f;
    float itemSize = iconSize + padding;

    if (m_CurrentPath != m_RootPath) {
        if (ImGui::Button("../")) {
            m_CurrentPath = m_CurrentPath.parent_path();
        }
    }

    ImVec2 panelSize = ImGui::GetContentRegionAvail();
    u32 numCols = (u32)(panelSize.x / itemSize);
    numCols = numCols > 0 ? numCols : 1;

    ImGui::Columns(numCols, nullptr, false);
    for (auto& p : getSortedDirectory(m_CurrentPath)) {
        std::filesystem::path itemPath = p.path();
        std::filesystem::path fileName = itemPath.filename();
        std::filesystem::path stem = itemPath.stem();
        std::filesystem::path extension = fileName.extension();
        bool isDirectory = p.is_directory();

        auto& manager = AssetManager::getInstance();

        // Continue if we don't support the file type
        if (!manager.isAssetRegistered(itemPath.string()) && !isDirectory && manager.isSupportedType(itemPath)) {
            DM_CORE_INFO("Registering Asset: {}", itemPath.string())
            manager.registerAsset(itemPath);
        }

        ImGui::PushID(itemPath.string().c_str());

        if (isDirectory) {
            // if (ImGui::ImageButton((ImTextureID)(uintptr_t)m_FolderIcon->getID(), ImVec2(iconSize, iconSize), { 0, 1 }, { 1, 0 })) {
            //     m_CurrentPath /= p.path().filename();
            // }
            ImGui::TextWrapped("%s", fileName.string().c_str());

            // Move on
            ImGui::PopID();
            ImGui::NextColumn();
            continue;
        }
        if (!manager.isSupportedType(itemPath)) {
            ImGui::PopID();
            continue;
        }

        BrowserItem currentItem;
        currentItem.handle = manager.getAssetHandleFromPath(itemPath.string());
        currentItem.type = manager.getMetaData(currentItem.handle).type;
        // renderItem(m_FileIcon, glm::vec2(iconSize), fileName.string(), currentItem);
        // if (ImGui::BeginPopupContextItem("FileContext")) {
        //     if (currentItem.type == AssetType::MODELSOURCE) {
        //         if (ImGui::MenuItem("Generate Model")) {
        //             ModelLoadSettings set;
        //             set.modelSource = currentItem.handle;
        //             auto pp = itemPath.parent_path() / (stem.string() + ".dmod");
        //             ModelSerializer::Serialize(pp, CreateRef<Model>(set));
        //             AssetManager::getInstance().registerAsset(pp);
        //         }
        //     }
        //     ImGui::EndPopup();
        // }
        ImGui::TextWrapped("%s", fileName.string().c_str());

        // Move on
        ImGui::PopID();

        ImGui::NextColumn();
    }
    ImGui::Columns(1);

    if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems)) {
        // TEMPORARY
        if (ImGui::MenuItem("New Scene")) {
            Utils::newFile(m_CurrentPath / "NewScene.dims", DefaultScene);
        }
        if (ImGui::MenuItem("New Material")) {
            Utils::newFile(m_CurrentPath / "NewMat.dmat", DefaultMaterial);
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

}
