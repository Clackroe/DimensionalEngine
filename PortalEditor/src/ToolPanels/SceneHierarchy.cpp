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

void SceneHierarchy::propertiesPanel()
{
    ImGui::Begin("Properties");
    if (m_SelectedEntity) {
        entityComponenets(m_SelectedEntity);
    }

    ImGui::End();
}

void SceneHierarchy::entityComponenets(Entity entity)
{
    if (entity.hasComponent<TransformComponent>()) {
        auto& component = entity.getComponent<TransformComponent>();
        customVec3Slider("Translation", component.Position);
        glm::vec3 rotation = glm::degrees(component.Rotation);
        customVec3Slider("Rotation", rotation);
        component.Rotation = glm::radians(rotation);
        customVec3Slider("Scale", component.Scale, 1.0f);
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

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
            m_SelectedEntity = {};
        }

        // Right-click on blank space
        // if (ImGui::BeginPopupContextWindow(0, 1)) {
        //     if (ImGui::MenuItem("Create Empty Entity"))
        //         m_SceneContext->createEntity("Empty Entity");
        //
        //     ImGui::EndPopup();
        // }
    }
    ImGui::End();

    propertiesPanel();
}
}
