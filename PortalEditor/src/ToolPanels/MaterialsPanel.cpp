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
    AssetMetaData meta = AssetManager::getInstance().getMetaData(handle);

    ImGui::Text("%s", meta.sourcePath.c_str());
    ImGui::Separator();

    AssetHandle alb = mat->getTexture(MaterialTexture::Albedo);
    Utils::assetDragDrop(alb, AssetType::TEXTURE, "Albedo");
    mat->setTexture(MaterialTexture::Albedo, alb);

    AssetHandle Normal = mat->getTexture(MaterialTexture::Normal);
    Utils::assetDragDrop(Normal, AssetType::TEXTURE, "Normal");
    mat->setTexture(MaterialTexture::Normal, Normal);

    AssetHandle Metal = mat->getTexture(MaterialTexture::Metalness);
    Utils::assetDragDrop(Metal, AssetType::TEXTURE, "Metal");
    mat->setTexture(MaterialTexture::Metalness, Metal);

    AssetHandle Rough = mat->getTexture(MaterialTexture::Roughness);
    Utils::assetDragDrop(Rough, AssetType::TEXTURE, "Rough");
    mat->setTexture(MaterialTexture::Roughness, Rough);

    AssetHandle AO = mat->getTexture(MaterialTexture::AO);
    Utils::assetDragDrop(AO, AssetType::TEXTURE, "AO");
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
