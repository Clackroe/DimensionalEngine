#include "Asset/Asset.hpp"
#include "Asset/MaterialSerializer.hpp"
#include "Asset/ModelSourceImporter.hpp"
#include "Core/Application.hpp"
#include "Log/log.hpp"
// #include "Rendering/SceneRenderer.hpp"
// #include "Rendering/SubMesh.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.hpp>
// #include "Rendering/SceneRenderer.hpp"
// #include "Rendering/OpenGL/OpenGL_Texture2D.hpp"
// #include "Rendering/RenderTarget.hpp"
// #include "Rendering/Texture2D.hpp"
// #include "Rendering/Texture2DManager.hpp"
#include "ToolPanels/ContentBrowser.hpp"
#include "ToolPanels/MaterialsPanel.hpp"
#include "core.hpp"
#include "imgui.h"
#include <Asset/AssetRegistrySerializer.hpp>
#include <KeyCodes.hpp>
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

// Ref<VAO> vao;
//
// Ref<VAO> quadVAO;
//
// Ref<Shader> shader;
//
// Ref<Shader> screenShader;
//
// Ref<Texture2D> tex;
//
// Ref<RenderTarget> target;
//
// Ref<GPUBuffer> buff;
//
// Ref<GPUBuffer> buffSS;
//
// Ref<SceneRenderer> renderer;
//
// Ref<SubMesh> sMesh;

static void testBedStart()
{
    std::vector<float> vertices = {
        0.5f, 0.5f, 0.0f, // top right
        1.0f, 0.0f, 0.0f, // top right
        1.0, 1.0,
        //
        0.5f, -0.5f, 0.0f, // bottom right
        0.0f, 1.0f, 0.0f, // top right
        1.0, 0.0,
        //
        -0.5f, -0.5f, 0.0f, // bottom left
        0.0f, 0.0f, 1.0f, // top right
        0.0, 0.0,
        //
        -0.5f, 0.5f, 0.0f, // top left
        1.0f, 0.0f, 1.0f, // top right
        0.0, 1.0,
        //
    };
    std::vector<u32> indices = {
        // note that we start from 0!
        0, 1, 3, // first Triangle
        1, 2, 3 // second Triangle
    };

    // SubMeshData sData;
    //
    // std::vector<Vertex> vs;
    // for (int i = 0; i < 4; i++) {
    //     Vertex v;
    //     v.Tangent = glm::vec3(0);
    //     v.Position = glm::vec3({ vertices[i * 8 + 0], vertices[i * 8 + 1], vertices[i * 8 + 2] });
    //     v.Normal = glm::vec3(0);
    //     v.BiTangent = glm::vec3(0);
    //     v.TexCoords = glm::vec2({ vertices[i * 8 + 6], vertices[i * 8 + 7] });
    //
    //     vs.push_back(v);
    // }
    // sData.vertices = vs;
    // sData.indices = indices;
    // sMesh = SubMesh::Create(sData);
    //
    // // Ref<SubMesh> test = ModelSourceImporter::loadModelSourceFromPath("Assets/Models/Cube.obj");
    //
    // std::vector<float> quadVertices = {
    //     // positions   // texCoords
    //     1.0f, 1.0f, 1.0f, 0.0f, // top-right
    //     1.0f, -1.0f, 1.0f, 1.0f, // bottom-right
    //     -1.0f, -1.0f, 0.0f, 1.0f, // bottom-left
    //     -1.0f, 1.0f, 0.0f, 0.0f, // top-left
    // };
    // std::vector<u32> quadIndices = {
    //     // note that we start from 0!
    //     0, 1, 3, // first Triangle
    //     1, 2, 3 // second Triangle
    // };
    //
    // quadVAO = VAO::Create();
    // VAOData dataQuad;
    // // dataQuad.indexBuffer = quadIndices;
    // // dataQuad.data = (const char*)quadVertices.data();
    // // dataQuad.dataSizeBytes = quadVertices.size() * sizeof(float);
    // // dataQuad.layout.push_back({ .type = AttributeType::FLOAT, .elementsCnt = 2, .normalized = false });
    // // dataQuad.layout.push_back({ .type = AttributeType::FLOAT, .elementsCnt = 2, .normalized = true });
    //
    // // quadVAO->SetData(dataQuad);
    //
    // vao = VAO::Create();
    //
    // // VAOData data;
    // // data.indexBuffer = indices;
    // // data.data = (const char*)vertices.data();
    // // data.dataSizeBytes = vertices.size() * sizeof(float);
    // // data.layout.push_back({ .type = AttributeType::FLOAT, .elementsCnt = 3, .normalized = false });
    // // data.layout.push_back({ .type = AttributeType::FLOAT, .elementsCnt = 3, .normalized = false });
    // // data.layout.push_back({ .type = AttributeType::FLOAT, .elementsCnt = 2, .normalized = true });
    // // vao->SetData(data);
    //
    // shader = Shader::Create("Assets/Shaders/testshader.glsl");
    //
    // Texture2DData texData;
    // texData.data = stbi_load("Assets/Textures/Albedo.png", (int*)&texData.width, (int*)&texData.height, (int*)&texData.channels, 0);
    // texData.format = TextureFormat::RGBA8;
    // tex = Texture2D::Create(texData);
    // tex->Bind(1);
    //
    // Renderer::SetClearColor({ 0.18, 0.18, 0.18 });
    //
    // RenderTargetData rtData;
    // rtData.width = Application::getApp().getWindowDM().getWidth();
    // rtData.height = Application::getApp().getWindowDM().getHeight();
    //
    // rtData.hdr = false;
    // rtData.attachments.push_back({ TextureFormat::DEFAULT });
    // rtData.attachments.push_back({ TextureFormat::DEFAULT });
    // rtData.attachments.push_back({ TextureFormat::DEFAULT });
    // rtData.depthAttachment = { TextureFormat::DEPTH16 };
    // target = RenderTarget::Create(rtData);
    //
    // // target->Bind();
    // // target->BindAttachment(0, 2);
    //
    // screenShader = Shader::Create("Assets/Shaders/Screen.glsl");
    //
    // struct alignas(16) Color {
    //     glm::vec3 col = glm::vec3(1.0, 0.0, 0.0);
    //     float _pad2;
    // };
}

static void testBedUpdate()
{
    // target->Bind();
    // // vao->Bind();
    // shader->Bind();
    // Renderer::ClearScreen(ClearBuffer::BOTH);
    // Renderer::DrawIndexed(vao, shader);
    // // vao->UnBind();
    // target->UnBind();
    //
    // // target->BindAttachment(0, 2);
    // // screenShader->Bind();
    // // // Renderer::ClearScreen(ClearBuffer::BOTH);
    // // Renderer::DrawIndexed(quadVAO, screenShader);
}

void PortalLayer::OnAttatch()
{

    DM_INFO("Portal Initialized");

    AssetManager& manager = AssetManager::getInstance();

    if (std::filesystem::exists("Assets/Registry.dreg")) {
        AssetRegistrySerializer::Deserialize("Assets/Registry.dreg", manager);
    }

    m_EditorCamera = EditorCamera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    m_EditorCamera.setPosition(glm::vec3 { 0, 0, 0 });
    // m_EditorCamera.setPosition(glm::vec3 { -8.0, 4.0, 10.0 });
    m_EditorCamera.setRotation(glm::quat(glm::radians(glm::vec3 { -15.0f, -30.0f, 0.0f })));

    s_Browser = CreateRef<ContentBrowser>("Assets");
    s_MatPanel = CreateRef<MaterialsPanel>();

    m_EditorScene = CreateRef<Scene>();
    m_ActiveScene = m_EditorScene;

    testBedStart();

    // m_SceneRenderer = CreateRef<SceneRenderer>(m_ActiveScene);
    setActiveScene(m_ActiveScene);

    // renderer = CreateRef<SceneRenderer>(m_ActiveScene);
}

void PortalLayer::setActiveScene(Ref<Scene> sc)
{
    m_ActiveScene = sc;
    // m_SceneRenderer->setScene(sc);
    m_HierarchyPanel.setSceneContext(sc);

    auto& app = Application::getApp();
    app.setSceneCTX(sc);

    auto& t = app.getScriptManager();
    // t.reloadGameLibrary("GameAppd.dll");
    t.reloadGameLibrary("Assets/Scripts/build/libGameApp.so");
}

void PortalLayer::startRuntime()
{
    if (!m_ActiveScene) {
        return;
    }

    m_RuntimeScene = CreateRef<Scene>();
    m_EditorScene->deepCopy(m_RuntimeScene);
    setActiveScene(m_RuntimeScene);

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

    m_RuntimeScene = nullptr;

    setActiveScene(m_EditorScene);
}

void PortalLayer::OnDetatch() { }

void PortalLayer::OnUpdate()
{

    testBedUpdate();
    m_EditorCamera.Update();
    glm::vec3 p = m_EditorCamera.getPosition();

    // CameraData data;
    // data.uCameraPosition = m_EditorCamera.getPosition();
    // data.fov = m_EditorCamera.getFOV();
    // data.proj = m_EditorCamera.getProjection();
    // data.view = m_EditorCamera.getViewMtx();
    // data.viewProj = m_EditorCamera.getViewProj();
    // data.farPlane = m_EditorCamera.m_FarClipPlane;
    // data.nearPlane = m_EditorCamera.m_NearClipPlane;
    // data.aspectRatio = m_EditorCamera.getAspectRatio();
    //
    // renderer->BeginFrame(data);

    if (m_ActiveScene) {

        if (m_State == EditorState::PLAY) {
            m_ActiveScene->updateSceneRuntime();
        }

        // m_SceneRenderer->beginScene(CameraData { m_EditorCamera.getViewProj(), p, m_EditorCamera.getAspectRatio(), m_EditorCamera.getViewMtx(), m_EditorCamera.getProjection(), m_EditorCamera.getFOV(), m_EditorCamera.m_NearClipPlane, m_EditorCamera.m_FarClipPlane });
        // m_SceneRenderer->render();
        // m_SceneRenderer->endScene();
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
        m_EditorScene = nScene;
        setActiveScene(m_EditorScene);
    }
}
void PortalLayer::saveCurrentScene()
{
    AssetMetaData sceneMeta = AssetManager::getInstance().getMetaData(m_ActiveSceneHandle);
    if (std::filesystem::exists(sceneMeta.sourcePath)) {
        auto matHandles = AssetManager::getInstance().getAssetHandles(AssetType::MATERIAL);
        for (auto handle : matHandles) {
            auto matMeta = AssetManager::getInstance().getMetaData(handle);
            // auto mat = AssetManager::getInstance().getAsset<Material>(handle);
            // MaterialSerializer::Serialize(matMeta.sourcePath, mat->getSettings());
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

    if (ImGui::Button("Reload Game Code")) {
        Application::getApp().setSceneCTX(m_ActiveScene);
        auto& app = Application::getApp();
        auto& t = app.getScriptManager();
        // t.reloadGameLibrary("GameAppd.dll");
        t.reloadGameLibrary("Assets/Scripts/build/libGameApp.so");
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
    // Ref<FrameBuffer> buf = m_SceneRenderer->getFrameBuffer();
    int att = 0;
    if (Input::isKeyDown(Key::F)) {
        att = 1;
    }

    // Ref<Texture2D> rtex = target->GetAttachment(att);
    // Renderer::ImGuiDrawTexture(rtex, viewportPanelSize.x, viewportPanelSize.y);

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
