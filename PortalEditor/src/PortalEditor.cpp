#include "Asset/Asset.hpp"
#include "Asset/MaterialSerializer.hpp"
#include "Rendering/SceneRenderer.hpp"
#include "ToolPanels/ContentBrowser.hpp"
#include "ToolPanels/MaterialsPanel.hpp"
#include "core.hpp"
#include "imgui.h"
#include <Asset/AssetRegistrySerializer.hpp>
#include <Input/KeyCodes.hpp>
#include <PortalEditor.hpp>
#include <Scene/Components.hpp>
#include <Scene/SceneSerializer.hpp>
#include <ToolPanels/Utils.hpp>
#include <dimensional.hpp>
#include <filesystem>
#include <string>

namespace Dimensional {

// TODO: Move to managed system
static std::string scenePath = "Assets/TestScene.dims";

static Ref<ContentBrowser> s_Browser;
static Ref<MaterialsPanel> s_MatPanel;

void PortalLayer::OnAttatch()
{

    DM_INFO("Portal Initialized");

    AssetManager& manager = AssetManager::getInstance();

    if (std::filesystem::exists("Assets/Registry.dreg")) {
        AssetRegistrySerializer::Deserialize("Assets/Registry.dreg", manager);
    }

    m_EditorCamera = EditorCamera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    m_EditorCamera.setPosition(glm::vec3 { -8.0, 4.0, 10.0 });
    m_EditorCamera.setRotation(glm::quat(glm::radians(glm::vec3 { -15.0f, -30.0f, 0.0f })));

    m_ActiveScene = CreateRef<Scene>();
    m_SceneRenderer = CreateRef<SceneRenderer>(m_ActiveScene);
    m_HierarchyPanel.setSceneContext(m_ActiveScene);

    s_Browser = CreateRef<ContentBrowser>("Assets");
    s_MatPanel = CreateRef<MaterialsPanel>();
}

void PortalLayer::startRuntime()
{
    if (!m_ActiveScene) {
        return;
    }
    m_ActiveScene->onSceneRuntimeStart();
    m_State = EditorState::PLAY;
}
void PortalLayer::endRuntime()
{
    if (!m_ActiveScene) {
        return;
    }
    m_ActiveScene->onSceneRuntimeEnd();
    m_State = EditorState::EDIT;
}

void PortalLayer::OnDetatch() { }

void PortalLayer::OnUpdate()
{
    m_EditorCamera.Update();
    glm::vec3 p = m_EditorCamera.getPosition();

    if (m_ActiveScene) {

        if (m_State == EditorState::PLAY) {
            m_ActiveScene->updateSceneRuntime();
        }

        m_SceneRenderer->beginScene(CameraData { m_EditorCamera.getViewProj(), p, m_EditorCamera.getAspectRatio(), m_EditorCamera.getViewMtx(), m_EditorCamera.getProjection(), m_EditorCamera.getFOV(), m_EditorCamera.m_NearClipPlane, m_EditorCamera.m_FarClipPlane });
        m_SceneRenderer->render();
        m_SceneRenderer->endScene();
    }

    if (Input::isKeyDown(Key::Escape)) {
        Application::getApp().stopApplication();
    }
}
void PortalLayer::openScene(AssetHandle sceneHandle)
{
    Ref<Scene> nScene = AssetManager::getInstance().getAsset<Scene>(sceneHandle);
    if (nScene) {
        m_ActiveSceneHandle = sceneHandle;
        m_ActiveScene = nScene;
        m_SceneRenderer->setScene(m_ActiveScene);
        m_HierarchyPanel.setSceneContext(m_ActiveScene);
    }
}
void PortalLayer::saveCurrentScene()
{
    AssetMetaData sceneMeta = AssetManager::getInstance().getMetaData(m_ActiveSceneHandle);
    if (std::filesystem::exists(sceneMeta.sourcePath)) {
        auto matHandles = AssetManager::getInstance().getAssetHandles(AssetType::MATERIAL);
        for (auto handle : matHandles) {
            auto matMeta = AssetManager::getInstance().getMetaData(handle);
            auto mat = AssetManager::getInstance().getAsset<Material>(handle);
            MaterialSerializer::Serialize(matMeta.sourcePath, mat->getSettings());
        }
        SceneSerializer::Serialize(sceneMeta.sourcePath, m_ActiveScene);

    } else {
        DM_CORE_WARN("Unable to save scene: {}", sceneMeta.sourcePath);
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
        saveCurrentScene();
    }

    if (ImGui::Button("Start/Stop")) {
        switch (m_State) {
        case EditorState::EDIT:
            startRuntime();
            break;
        case EditorState::PLAY:
            endRuntime();
            break;
        }
    }

    switch (m_State) {
    case EditorState::EDIT:
        ImGui::Text("Editing");
        break;
    case EditorState::PLAY:
        ImGui::Text("Playing");
        break;
    }

    ImGui::End();

    s_Browser->renderImGui();
    s_MatPanel->renderImGui();

    // TEST ASSETMANAGER
    ImGui::Begin("Asset Registry");
    ImVec2 availableSize = ImGui::GetContentRegionAvail();
    float listBoxHeight = availableSize.y - ImGui::GetFrameHeightWithSpacing();
    AssetManager& manager = AssetManager::getInstance();
    if (ImGui::BeginListBox("##RegistryListBox", ImVec2(-FLT_MIN, listBoxHeight))) {
        for (auto& [h, meta] : manager.m_Registry) {
            std::string handleStr = std::to_string(static_cast<uint64_t>(h));
            std::string typeStr = Asset::assetTypeToString(meta.type);

            if (!meta.sourcePath.empty() && !typeStr.empty()) {
                ImGui::Text("Handle: %s", handleStr.c_str());
                ImGui::Text("Path: %s", meta.sourcePath.c_str());
                ImGui::Text("Type: %s", typeStr.c_str());
                ImGui::Separator();
            } else {
                DM_WARN("Invalid metadata encountered for handle %s", handleStr.c_str());
            }
        }
        ImGui::EndListBox();
    }

    ImGui::End();

    // Viewport
    ImGui::Begin("Viewport");
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    if ((viewportPanelSize.x != m_ViewPortSize.x) || (viewportPanelSize.y != m_ViewPortSize.y)) {
        m_EditorCamera.setViewportDimensions(viewportPanelSize.x, viewportPanelSize.y);
        m_ViewPortSize = { viewportPanelSize.x, viewportPanelSize.y };
    }
    Ref<FrameBuffer> buf = m_SceneRenderer->getFrameBuffer();
    int att = 0;
    if (Input::isKeyDown(Key::F)) {
        att = 1;
    }
    ImGui::Image(reinterpret_cast<ImTextureID>(buf->getAttachmentID(att)), ImVec2 { viewportPanelSize.x, viewportPanelSize.y }, { 0, 1 }, { 1, 0 });

    AssetHandle newHandle = m_ActiveSceneHandle;
    UI::assetDragDrop(newHandle, AssetType::SCENE);
    if (newHandle != m_ActiveSceneHandle) {
        openScene(newHandle);
    }

    ImGui::End();
    //

    ImGui::End();
}
};
