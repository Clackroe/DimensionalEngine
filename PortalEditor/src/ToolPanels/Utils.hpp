#include <Assets/Asset.hpp>
#include <Assets/AssetManager.hpp>
#include <Assets/AssetMeta.hpp>
#include <core.hpp>
#include <imgui.h>
namespace Dimensional {
namespace UI {
    static void assetDragDrop(AssetHandle& handle, AssetType typeOfAsset)
    {
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
}
