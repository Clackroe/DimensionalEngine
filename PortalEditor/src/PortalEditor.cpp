#include "Core/Assets/AssetManager.hpp"
#include "Input/KeyCodes.hpp"
#include "Scene/Components.hpp"
#include "imgui.h"
#include <PortalEditor.hpp>
#include <Scene/SceneSerializer.hpp>

namespace Dimensional {

// TODO: Move to managed system
static std::string scenePath = "Assets/TestScene.dmsn";

void PortalLayer::OnAttatch()
{

    DM_INFO("Portal Initialized");

    m_EditorCamera = EditorCamera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    m_EditorCamera.setPosition(glm::vec3 { -8.0, 4.0, 10.0 });
    m_EditorCamera.setRotation(glm::quat(glm::radians(glm::vec3 { -15.0f, -30.0f, 0.0f })));

    m_ActiveScene = CreateRef<Scene>();
}
void PortalLayer::OnDetatch() { }

void PortalLayer::OnUpdate()
{
    m_EditorCamera.Update();
    glm::vec3 p = m_EditorCamera.getPosition();

    if (m_ActiveScene) {
        m_ActiveScene->beginScene();

        Renderer::beginScene(CameraData { m_EditorCamera.getViewProj(), p, m_EditorCamera.getViewMtx(), m_EditorCamera.getProjection() });

        m_ActiveScene->updateEditor();

        Renderer::endScene();
    }

    if (Input::IsKeyDown(Key::Escape)) {
        Application::getApp().stopApplication();
    }
}
void PortalLayer::OnImGuiRender()
{

    // Note: Switch this to true to enable dockspace
    static bool useDocking = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", &useDocking, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    m_HierarchyPanel.renderImGui();

    // TEST SCENE SAVING
    ImGui::Begin("Test Saving");
    if (ImGui::Button("Save")) {
        SceneSerializer::Serialize(scenePath, m_ActiveScene);
    }
    if (ImGui::Button("Load")) {
        Ref<Scene> nScene = CreateRef<Scene>();
        SceneSerializer::Deserialize(scenePath, nScene);
        m_ActiveScene = nScene;
        m_HierarchyPanel.setSceneContext(m_ActiveScene);
    }

    ImGui::End();

    // Viewport
    ImGui::Begin("Viewport");
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    if ((viewportPanelSize.x != m_ViewPortSize.x) || (viewportPanelSize.y != m_ViewPortSize.y)) {
        m_EditorCamera.setViewportDimensions(viewportPanelSize.x, viewportPanelSize.y);
        m_ViewPortSize = { viewportPanelSize.x, viewportPanelSize.y };
    }
    Ref<FrameBuffer> buf = Renderer::getFrameBuffer();

    ImGui::Image(reinterpret_cast<ImTextureID>(buf->getAttachmentID(0)), ImVec2 { viewportPanelSize.x, viewportPanelSize.y }, { 0, 1 }, { 1, 0 });
    ImGui::End();
    //

    ImGui::End();
}
};
