#include "EngineAPI.hpp"
#include "ImGui/ImGuiLayer.hpp"
#include "Log/log.hpp"
#include "Rendering/RenderTarget.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Texture2D.hpp"
#include "Rendering/TextureEnums.hpp"
#include "Rendering/VAO.hpp"
#include "Scripting/NativeScriptManager.hpp"
#include "core.hpp"
#include <Core/Application.hpp>

#include <stb_image.hpp>

#include <Core/Time.hpp>

#include <glad.h>

namespace Dimensional {

Ref<VAO> vao;

Ref<Shader> shader;

Ref<Texture2D> tex;

Ref<RenderTarget> target;

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

    vao = VAO::Create();

    VAOData data;
    data.indexBuffer = indices;
    data.data = (const char*)vertices.data();
    data.dataSizeBytes = vertices.size() * sizeof(float);
    data.layout.push_back({ .type = AttributeType::FLOAT, .elementsCnt = 3, .normalized = false });
    data.layout.push_back({ .type = AttributeType::FLOAT, .elementsCnt = 3, .normalized = false });
    data.layout.push_back({ .type = AttributeType::FLOAT, .elementsCnt = 2, .normalized = true });

    vao->SetData(data);

    shader = Shader::Create("Assets/Shaders/testshader.glsl");
    shader->Bind();

    Texture2DData texData;
    texData.data = stbi_load("Assets/Textures/Albedo.png", (int*)&texData.width, (int*)&texData.height, (int*)&texData.channels, 0);
    texData.format = TextureFormat::RGBA8;
    tex = Texture2D::Create(texData);
    tex->Bind(1);

    Renderer::SetClearColor({ 0.18, 0.18, 0.18 });

    RenderTargetData rtData;
    rtData.width = Application::getApp().getWindowDM().getWidth();
    rtData.height = Application::getApp().getWindowDM().getHeight();

    rtData.hdr = false;
    rtData.attachments.push_back({ TextureFormat::DEFAULT });
    rtData.attachments.push_back({ TextureFormat::DEFAULT });
    rtData.depthAttachment = { TextureFormat::DEPTH16 };
    target = RenderTarget::Create(rtData);
    target->Bind();
}

static void testBedUpdate()
{
    Renderer::ClearScreen(ClearBuffer::BOTH);

    Renderer::DrawIndexed(vao, shader);
}

Application* Application::s_Application = nullptr;

Application::Application(const std::string& title, u32 width, u32 height)
{
    Log::Init(); // Logging needs to be initialized sooner than the other systems
    //
    DM_CORE_INFO("TEST");

    DM_CORE_ASSERT(!s_Application, "Application already created!! Aborting.");
    s_Application = this;

    m_Window = CreateScope<Window>(WindowSettings { width, height, title });

    // m_ImGuiOverlay = new ImGuiLayer();
    // m_LayerStack.pushOverlay(m_ImGuiOverlay);

    DM_CORE_INFO("Platform: {0}", DM_PLATFORM);

    initializeSubSystems();
    // m_ScriptManager.reloadGameLibrary("Assets/Scripts/build/libGameApp.so");
    testBedStart();
}

static float frameTime = 0;
void Application::runApplication()
{

    while (m_Running) {
        float frameStartTime = Time::getTime();
        Time::Update();

        EventSystem::ProcessEvents();

        //------Update Layers-------
        for (Layer* layer : m_LayerStack) {
            layer->OnUpdate();
        }

        //------Update imgui Layers-------
        // m_ImGuiOverlay->beginFrame();
        //
        // // ImGui::Begin("Stats");
        // // ImGui::Text("FPS: %f", 1 / Time::deltaTime());
        // // ImGui::End();
        //
        // for (Layer* layer : m_LayerStack) {
        //     layer->OnImGuiRender();
        // }
        //
        // m_ImGuiOverlay->endFrame();

        //------
        testBedUpdate();
        m_Window->update();

        frameTime = Time::getTime() - frameStartTime;
    }

    tex = nullptr;
    target = nullptr;
    vao = nullptr;
}

void Application::initializeSubSystems()
{
    m_EventSystem.Init();
    Input::Init();

    m_ScriptManager.freeGameLibrary();
}

Ref<Scene> Application::getSceneCTX()
{
    if (!m_SceneCTX) {
        DM_CORE_ERROR("Tried to acces null scene context");
        return nullptr;
    }
    return m_SceneCTX;
}

}
