#include "Assets/AssetManager.hpp"
#include "Assets/AssetMeta.hpp"
#include "Log/log.hpp"
#include "Rendering/Material.hpp"
#include "imgui.h"
#include <ToolPanels/MaterialsPanel.hpp>
#include <imgui_internal.h>
namespace Dimensional {

namespace Utils {
    static void assetDragDrop(AssetHandle& handle, AssetType typeOfAsset, const std::string& label)
    {
        ImGui::Text("%s", label.c_str());
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ASSET")) {
                AssetHandle droppedHandle = *(AssetHandle*)payload->Data;
                AssetType assetsType = AssetManager::getInstance().getMetaData(droppedHandle).type;

                if (typeOfAsset == assetsType) {
                    handle = droppedHandle;
                } else {
                    DM_CORE_WARN("Wrong AssetType {0} {1}", Asset::assetTypeToString(typeOfAsset), Asset::assetTypeToString(assetsType))
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
}

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
    Utils::assetDragDrop(alb, AssetType::TEXTURE, label);
    mat->setTexture(MaterialTexture::Albedo, alb);

    AssetHandle Normal = mat->getTexture(MaterialTexture::Normal);
    ImGui::TextWrapped("Normal");
    label = std::filesystem::path(AssetManager::getInstance().getMetaData(Normal).sourcePath).stem().string();
    Utils::assetDragDrop(Normal, AssetType::TEXTURE, label);
    mat->setTexture(MaterialTexture::Normal, Normal);

    AssetHandle Metal = mat->getTexture(MaterialTexture::Metalness);
    ImGui::TextWrapped("Metalness");
    label = std::filesystem::path(AssetManager::getInstance().getMetaData(Metal).sourcePath).stem().string();
    Utils::assetDragDrop(Metal, AssetType::TEXTURE, label);
    mat->setTexture(MaterialTexture::Metalness, Metal);

    AssetHandle Rough = mat->getTexture(MaterialTexture::Roughness);
    ImGui::TextWrapped("Roughness");
    label = std::filesystem::path(AssetManager::getInstance().getMetaData(Rough).sourcePath).stem().string();
    Utils::assetDragDrop(Rough, AssetType::TEXTURE, label);
    mat->setTexture(MaterialTexture::Roughness, Rough);

    AssetHandle AO = mat->getTexture(MaterialTexture::AO);
    ImGui::TextWrapped("AO");
    label = std::filesystem::path(AssetManager::getInstance().getMetaData(AO).sourcePath).stem().string();
    Utils::assetDragDrop(AO, AssetType::TEXTURE, label);
    mat->setTexture(MaterialTexture::AO, AO);
};

void MaterialsPanel::renderImGui()
{
    std::vector<AssetHandle> materialHandles;
    materialHandles = AssetManager::getInstance().getAssetHandles(AssetType::MATERIAL);

    ImGui::Begin("Materials");

    if (!materialHandles.empty()) {

        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

        for (auto handle : materialHandles) {
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
