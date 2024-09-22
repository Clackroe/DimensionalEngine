#include "Assets/AssetManager.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/ModelSource.hpp"
#include "Scene/Components.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneSerializer.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <ToolPanels/SceneHierarchy.hpp>
namespace Dimensional {

// Currently this function is copied from TheCherno's Tutorial series.
// TODO: Rewrite this function in Dimensional's style
static void customVec3Slider(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 0, 0 });

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 { 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 { 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 { 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize))
        values.x = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 { 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 { 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 { 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize))
        values.y = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 { 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 { 0.2f, 0.35f, 0.9f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 { 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize))
        values.z = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
}

// static std::string assetChooser(AssetType type, std::string current)
// {
//     auto assets = AssetManager::getAllAssetNames(type);
//     if (ImGui::Button(current.c_str())) {
//         ImGui::OpenPopup("Choose Asset");
//     }
//     bool shouldRemoveComponent = false;
//     if (ImGui::BeginPopup("Choose Asset")) {
//         for (auto n : assets) {
//             if (ImGui::MenuItem(n.c_str())) {
//                 return n;
//             }
//         }
//         ImGui::EndPopup();
//     }
//     return "";
// }

SceneHierarchy::SceneHierarchy(Ref<Scene> scene)
    : m_SceneContext(scene)
{
}

void SceneHierarchy::setSceneContext(Ref<Scene> scene)
{
    m_SceneContext = scene;
    m_SelectedEntity = {};
}

void SceneHierarchy::entityTreeNode(Entity entity)
{
    std::string& tag = entity.getComponent<TagComponent>().Tag;
    ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
    bool isOpened = ImGui::TreeNodeEx((void*)(u64)(u32)entity, flags, "%s", tag.c_str());
    if (ImGui::IsItemClicked()) {
        m_SelectedEntity = entity;
    }

    bool entityDeleted = false;
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete Entity"))
            entityDeleted = true;

        ImGui::EndPopup();
    }
    if (isOpened) {
        ImGui::TreePop();
    }

    if (entityDeleted) {
        m_SceneContext->destroyEntity(entity);
        if (m_SelectedEntity == entity) {
            m_SelectedEntity = {};
        }
    }
}
template <typename CType, typename CSpecificFunction>
void SceneHierarchy::componentNode(const std::string& name, Entity entity, CSpecificFunction function, bool canBeRemoved)
{
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
    if (entity.hasComponent<CType>()) {
        auto& component = entity.getComponent<CType>();
        ImVec2 availableRegion = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 4, 4 });
        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::Separator();
        bool open = ImGui::TreeNodeEx((void*)(typeid(CType).hash_code()), treeNodeFlags, "%s", name.c_str());
        ImGui::PopStyleVar();

        ImGui::SameLine(availableRegion.x - lineHeight * 0.5f);
        if (ImGui::Button("~", ImVec2 { lineHeight, lineHeight })) {
            ImGui::OpenPopup("Component Settings");
        }
        bool shouldRemoveComponent = false;
        if (ImGui::BeginPopup("Component Settings")) {
            if (canBeRemoved) {
                if (ImGui::MenuItem("Remove Component")) {
                    shouldRemoveComponent = true;
                }
            }
            ImGui::EndPopup();
        }
        if (open) {
            function(component);
            ImGui::TreePop();
        }

        if (shouldRemoveComponent) {
            entity.removeComponent<CType>();
        }
    }
}

void SceneHierarchy::propertiesPanel()
{
    ImGui::Begin("Properties");
    if (m_SelectedEntity) {
        entityComponents(m_SelectedEntity);
    }

    ImGui::End();
}

void SceneHierarchy::entityComponents(Entity entity)
{
    if (entity.hasComponent<TagComponent>()) {
        auto& tag = entity.getComponent<TagComponent>().Tag;

        char buffer[256];
        std::memset(buffer, 0, sizeof(buffer));
        std::strncpy(buffer, tag.c_str(), sizeof(buffer) - 1);
        if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
            tag = std::string(buffer);
        }
    }

    ImGui::SameLine();
    ImGui::PushItemWidth(-1);

    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("AddComponent");

    if (ImGui::BeginPopup("AddComponent")) {
        if (ImGui::Button("Mesh")) {
            entity.addComponent<MeshRenderer>();
        }
        // DisplayAddComponentEntry<TextComponent>("Text Component");

        ImGui::EndPopup();
    }

    ImGui::PopItemWidth();

    componentNode<TransformComponent>(
        "Transform", entity, [](auto& component) {
        customVec3Slider("Position", component.Position);
        glm::vec3 rotation = glm::degrees(component.Rotation);
        customVec3Slider("Rotation", rotation);
        component.Rotation = glm::radians(rotation);
        customVec3Slider("Scale", component.Scale, 1.0f); }, false);

    if (entity.hasComponent<PointLightComponent>()) {
        componentNode<PointLightComponent>(
            "Point Light", entity, [](auto& component) {
                ImGui::ColorEdit3("Color", glm::value_ptr(component.color));
                ImGui::DragFloat("Intensity", &component.intensity, 0.5f, 0.0f, 30.0f);
                ImGui::DragFloat("Constant", &component.constant, 0.5f, 1.0f, 10.0f);
                ImGui::DragFloat("Linear", &component.linear, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Quadratic", &component.quadratic, 0.01f, 0.0f, 1.0f);
            },
            true);
    }
    if (entity.hasComponent<SpotLightComponent>()) {
        componentNode<SpotLightComponent>(
            "Spot Light", entity, [](auto& component) {
                ImGui::ColorEdit3("Color", glm::value_ptr(component.color));
                ImGui::DragFloat("Cut Off (degrees)", &component.cutOff, 1.0f, 0.0f, 90.0f);
                ImGui::DragFloat("Outer Cut Off (degrees)", &component.outerCutOff, 1.0f, 0.0f, 90.0f);
                ImGui::DragFloat("Intensity", &component.intensity, 0.5f, 0.0f, 30.0f);
                ImGui::DragFloat("Constant", &component.constant, 0.5f, 1.0f, 10.0f);
                ImGui::DragFloat("Linear", &component.linear, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Quadratic", &component.quadratic, 0.01f, 0.0f, 1.0f);
            },
            true);
    }
    if (entity.hasComponent<MeshRenderer>()) {
        componentNode<MeshRenderer>(
            "Model Renderer", entity, [](MeshRenderer& component) {
                Ref<Model> model = AssetManager::getInstance().getAsset<Model>(component.model);

                // ImGui::InputScalar("ID: ", ImGuiDataType_U64, &component.model);
                ImGui::Text("Model!!");
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ASSET")) {
                    AssetHandle handle = *(AssetHandle*)payload->Data;
                    component.model = handle;
                }

                if (model) {
                    auto mats = model->getMaterials();

                    for (auto& m : mats) {

                        Ref<Material> mat1 = AssetManager::getInstance().getAsset<Material>(m);
                        ImGui::BeginListBox("Mesh");
                        u64 id = mat1->getTexture(MaterialTexture::Albedo);
                        ImGui::InputScalar("Albedo: ", ImGuiDataType_U64, &id);

                        mat1->setTexture(MaterialTexture::Albedo, id);
                    }

                    ImGui::EndListBox();
                }
            },
            true);
    }
}

void SceneHierarchy::renderImGui()
{

    ImGui::Begin("Hierarchy");

    if (m_SceneContext) {
        for (auto entity : m_SceneContext->m_Registry.view<entt::entity>()) {
            Entity e = { entity, m_SceneContext.get() };
            entityTreeNode(e);
        };

        // Right-click on blank space
        if (ImGui::BeginPopupContextWindow(0, 1)) {
            if (ImGui::MenuItem("Create Empty Entity"))
                m_SceneContext->createEntity("Empty Entity");

            ImGui::EndPopup();
        }
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
            m_SelectedEntity = {};
        }
    }
    ImGui::End();

    propertiesPanel();
}
}
