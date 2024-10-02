#include "Assets/AssetManager.hpp"
#include "Assets/AssetMeta.hpp"
#include "Log/log.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/ModelSource.hpp"
#include "Scene/Components.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneSerializer.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <ToolPanels/SceneHierarchy.hpp>
#include <ToolPanels/Utils.hpp>
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

    if (ImGui::BeginPopupContextItem("Entity Context")) {
        if (ImGui::MenuItem("Delete Entity")) {
            m_SceneContext->destroyEntity(entity);
            if (m_SelectedEntity == entity) {
                m_SelectedEntity = {};
            }
        }
        ImGui::EndPopup();
    }
    if (isOpened) {
        ImGui::TreePop();
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
        if (ImGui::Button("Point Light")) {
            entity.addComponent<PointLightComponent>();
        }
        if (ImGui::Button("Spot Light")) {
            entity.addComponent<SpotLightComponent>();
        }
        if (ImGui::Button("Sky Light")) {
            entity.addComponent<SkyLight>();
        }
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
    if (entity.hasComponent<SkyLight>()) {
        componentNode<SkyLight>(
            "Sky Light", entity, [](auto& component) {
                ImGui::Text("Environment Map:");
                UI::assetDragDrop(component.envMap, AssetType::ENVIRONMENTMAP,
                    std::filesystem::path(AssetManager::getInstance().getMetaData(component.envMap).sourcePath).stem().string());

                ImGui::DragFloat("Linear", &component.lod, 0.01f, 0.0f, 4.0f);
            },
            true);
    }

    if (entity.hasComponent<MeshRenderer>()) {
        componentNode<MeshRenderer>(
            "Model Renderer", entity, [](MeshRenderer& component) {
                AssetHandle modelID = component.model;
                ImGui::Text("Model:");
                const std::string& p = AssetManager::getInstance().getMetaData(modelID).sourcePath;
                UI::assetDragDrop(modelID, AssetType::MODEL,
                    std::filesystem::path(p).stem().string());
                if (modelID != component.model) {
                    component.setModelHandle(modelID);
                }

                Ref<Model> model = AssetManager::getInstance().getAsset<Model>(component.model);
                if (!model) {
                    return;
                }

                Ref<ModelSource> source = AssetManager::getInstance().getAsset<ModelSource>(model->getSource());
                if (source) {
                    ImGui::Separator();

                    ImVec2 availableSize = ImGui::GetContentRegionAvail();
                    float listBoxHeight = availableSize.y - ImGui::GetFrameHeightWithSpacing();

                    // Materials Section
                    if (ImGui::BeginListBox("##MaterialOverrides", ImVec2(-FLT_MIN, listBoxHeight))) {
                        ImGui::Text("Materials:");
                        ImGui::Separator();

                        // Loop through materials and allow override
                        for (u32 i = 0; i < source->getMaterialHandles().size(); i++) {
                            ImGui::PushID(i);

                            if (component.materialOverrides[i] != source->getMaterialHandles()[i] && (u64)component.materialOverrides[i] != 0) {

                                if (ImGui::Button("Reset##ResetButton")) {
                                    component.materialOverrides[i] = source->getMaterialHandles()[i];
                                }
                                ImGui::SameLine();
                            }

                            ImGui::Text("%d:", i + 1);
                            ImGui::SameLine();

                            AssetHandle matID = component.materialOverrides[i];
                            UI::assetDragDrop(matID, AssetType::MATERIAL,
                                std::filesystem::path(AssetManager::getInstance().getMetaData(
                                                                                     (u64)component.materialOverrides[i] == 0
                                                                                         ? source->getMaterialHandles()[i]
                                                                                         : component.materialOverrides[i])
                                                          .sourcePath)
                                    .stem()
                                    .string());

                            if (matID != component.materialOverrides[i]) {
                                component.materialOverrides[i] = matID;
                            }

                            if (ImGui::IsItemHovered()) {
                                ImGui::SetTooltip("Drag and drop to assign a material.");
                            }

                            ImGui::PopID();
                        }

                        ImGui::EndListBox();
                    }
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
            ImGui::PushID(e.getID());
            entityTreeNode(e);
            ImGui::PopID();
        };

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
            m_SelectedEntity = {};
        }

        // Right-click on blank space
        if (ImGui::IsMouseDown(1) && ImGui::IsWindowHovered() && !m_SelectedEntity) {
            ImGui::OpenPopup("Add Entity");
        }

        if (ImGui::BeginPopup("Add Entity")) {
            if (ImGui::MenuItem("Create Empty Entity")) {
                m_SceneContext->createEntity("Empty Entity");
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();

    propertiesPanel();
}
}
