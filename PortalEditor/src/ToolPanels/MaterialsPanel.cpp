#include "Asset/AssetManager.hpp"
#include "Asset/AssetMeta.hpp"
#include "Log/log.hpp"
#include "Rendering/Material.hpp"
#include "imgui.h"
#include <ToolPanels/MaterialsPanel.hpp>
#include <ToolPanels/Utils.hpp>
#include <imgui_internal.h>
namespace Dimensional {

static void renderMaterialTool(AssetHandle handle)
{
    Ref<Material> mat = AssetManager::getInstance().getAsset<Material>(handle);
    if (!mat) {
        return;
    }
    AssetMetaData meta = AssetManager::getInstance().getMetaData(handle);

    ImGui::Text("%s", meta.sourcePath.c_str());
    ImGui::Separator();

    AssetHandle alb = mat->getTexture(MaterialTexture::Albedo);
    ImGui::TextWrapped("Albedo");
    std::string label = std::filesystem::path(AssetManager::getInstance().getMetaData(alb).sourcePath).stem().string();
    UI::assetDragDrop(alb, AssetType::TEXTURE, label);
    mat->setTexture(MaterialTexture::Albedo, alb);

    AssetHandle Normal = mat->getTexture(MaterialTexture::Normal);
    ImGui::TextWrapped("Normal");
    label = std::filesystem::path(AssetManager::getInstance().getMetaData(Normal).sourcePath).stem().string();
    UI::assetDragDrop(Normal, AssetType::TEXTURE, label);
    mat->setTexture(MaterialTexture::Normal, Normal);

    AssetHandle Metal = mat->getTexture(MaterialTexture::Metalness);
    ImGui::TextWrapped("Metalness");
    label = std::filesystem::path(AssetManager::getInstance().getMetaData(Metal).sourcePath).stem().string();
    UI::assetDragDrop(Metal, AssetType::TEXTURE, label);
    mat->setTexture(MaterialTexture::Metalness, Metal);

    AssetHandle Rough = mat->getTexture(MaterialTexture::Roughness);
    ImGui::TextWrapped("Roughness");
    label = std::filesystem::path(AssetManager::getInstance().getMetaData(Rough).sourcePath).stem().string();
    UI::assetDragDrop(Rough, AssetType::TEXTURE, label);
    mat->setTexture(MaterialTexture::Roughness, Rough);

    AssetHandle AO = mat->getTexture(MaterialTexture::AO);
    ImGui::TextWrapped("AO");
    label = std::filesystem::path(AssetManager::getInstance().getMetaData(AO).sourcePath).stem().string();
    UI::assetDragDrop(AO, AssetType::TEXTURE, label);
    mat->setTexture(MaterialTexture::AO, AO);
};

void MaterialsPanel::renderImGui()
{
    std::vector<AssetHandle> materialHandles;
    materialHandles = AssetManager::getInstance().getAssetHandles(AssetType::MATERIAL);

    static char searchQuery[256] = ""; // Search query buffer

    ImGui::Begin("Materials");

    // Search bar
    ImGui::InputTextWithHint("##Search", "Search materials...", searchQuery, sizeof(searchQuery));

    if (!materialHandles.empty()) {

        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

        for (auto handle : materialHandles) {
            AssetMetaData meta = AssetManager::getInstance().getMetaData(handle);
            std::string materialName = std::filesystem::path(meta.sourcePath).stem().string();

            // Filter the materials based on the search query
            if (strlen(searchQuery) > 0 && materialName.find(searchQuery) == std::string::npos) {
                continue;
            }

            ImGui::PushID(handle);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 4, 4 });
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*)(typeid(materialHandles).hash_code()), treeNodeFlags, "%llu", (u64)materialHandles[0]);
            ImGui::PopStyleVar();
            if (open) {
                renderMaterialTool(handle);
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
    }

    ImGui::End();
}

}
