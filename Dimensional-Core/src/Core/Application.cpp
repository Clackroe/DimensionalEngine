#include "EngineAPI.hpp"
#include "GLFW/glfw3.h"
#include "ImGui/ImGuiLayer.hpp"
#include "Log/log.hpp"
#include "Rendering/GPUBuffer.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/Shader.hpp"
#include "Scripting/NativeScriptManager.hpp"
#include "core.hpp"
#include "nvrhi/nvrhi.h"
#include "nvrhi/utils.h"
#include <Core/Application.hpp>

#include <stb_image.hpp>

#include <Core/Time.hpp>

namespace Dimensional {

Application* Application::s_Application = nullptr;

nvrhi::CommandListHandle cmd;
Ref<DeviceManager> dm_dev;
nvrhi::IDevice* dev;

Ref<Shader> shader;

Ref<Pipeline> pipe;

nvrhi::FramebufferHandle fb;

nvrhi::TextureHandle textureTest1;

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
};

static const Vertex g_Vertices[] = {
    //  position
    { { 0.f, -0.5f, 0.f }, { 0, 1 } },
    { { 0.5f, 0.5f, 0.f }, { 1, 0 } },
    { { -0.5f, 0.5f, 0.f }, { 1, 1 } },
};

nvrhi::BufferHandle vertexBuffer;
nvrhi::GraphicsPipelineHandle graphicsPipeline;
static void tempInit()
{
    cmd = dev->createCommandList();

    shader = Shader::Create("Assets/Shaders/nvrhi-test.glsl");

    nvrhi::TextureDesc td;
    td.debugName = "Att 1";
    td.setFormat(nvrhi::Format::RGB32_FLOAT);
    td.setWidth(Application::getApp().getWindowDM().getWidth());
    td.setHeight(Application::getApp().getWindowDM().getHeight());
    td.setInitialState(nvrhi::ResourceStates::ShaderResource);
    td.setKeepInitialState(true);
    td.arraySize = 1;

    textureTest1 = dev->createTexture(td);
    if (!textureTest1) {
        DM_CORE_ERROR("Failed to create tex1")
    }

    td.debugName += "2";
    auto textureTest2 = dev->createTexture(td);
    if (!textureTest2) {
        DM_CORE_ERROR("Failed to create tex2")
    }

    auto framebufferDesc = nvrhi::FramebufferDesc()
                               .addColorAttachment(textureTest2);

    nvrhi::FramebufferHandle framebuffer = dev->createFramebuffer(framebufferDesc);
    if (!framebuffer) {
        DM_CORE_ERROR("Failed to create framebuff")
    }

    {
        PipelineCreateInfo info;
        info.shader = shader;
        pipe = Pipeline::Create(info);
        pipe->SetInput(0, 0, textureTest1, nvrhi::ResourceType::Texture_SRV);
        pipe->Compile(framebuffer);
    }

    auto vertexBufferDesc = nvrhi::BufferDesc()
                                .setByteSize(sizeof(g_Vertices))
                                .setIsVertexBuffer(true)
                                .setInitialState(nvrhi::ResourceStates::VertexBuffer)
                                .setKeepInitialState(true) // enable fully automatic state tracking
                                .setDebugName("Vertex Buffer");

    vertexBuffer = dev->createBuffer(vertexBufferDesc);

    cmd->open();
    const uint32_t width = 1920;
    const uint32_t height = 1080;
    const size_t textureRowPitch = width * 3 * sizeof(float); // 3 floats per pixel

    // Create a buffer filled with green (0.0f, 1.0f, 0.0f)
    std::vector<float> textureData(width * height * 3, 0.0f);
    for (size_t i = 0; i < width * height; ++i) {
        textureData[i * 3 + 0] = 0.0f; // R
        textureData[i * 3 + 1] = 1.0f; // G
        textureData[i * 3 + 2] = 0.0f; // B
    }

    // Then write it to the texture
    cmd->writeTexture(textureTest1,
        /* arraySlice = */ 0,
        /* mipLevel = */ 0,
        textureData.data(),
        textureRowPitch);

    cmd->close();
    dev->executeCommandList(cmd);

    cmd->open();
    cmd->writeBuffer(vertexBuffer, g_Vertices, sizeof(g_Vertices));
    cmd->close();
    dev->executeCommandList(cmd);
};

static void tempUpdate()
{
    cmd->open();

    fb = dm_dev->GetCurrentFramebuffer();
    nvrhi::utils::ClearColorAttachment(cmd, fb, 0, { static_cast<float>(std::sin(glfwGetTime())), static_cast<float>(std::sin(glfwGetTime())), 0.5, 1 });

    nvrhi::VertexBufferBinding t;
    t.setSlot(0);
    t.setBuffer(vertexBuffer);
    t.setOffset(0);

    auto graphicsState = nvrhi::GraphicsState();

    pipe->Compile(fb);
    graphicsState = pipe->Bind(graphicsState);

    graphicsState.setFramebuffer(fb).setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(Application::getApp().getWindowDM().getWidth(), Application::getApp().getWindowDM().getHeight())));
    graphicsState.addVertexBuffer(t);

    cmd->setGraphicsState(graphicsState);

    // Draw our geometry
    auto drawArguments = nvrhi::DrawArguments()
                             .setVertexCount(std::size(g_Vertices));

    cmd->draw(drawArguments);

    // Close and execute the command list
    cmd->close();
    dev->executeCommandList(cmd);
};

Application::Application(const std::string& title, u32 width, u32 height)
{
    Log::Init(); // Logging needs to be initialized sooner than the other systems
    //

    DM_CORE_ASSERT(!s_Application, "Application already created!! Aborting.");
    s_Application = this;

    m_Window = CreateScope<Window>(WindowSettings { width, height, title });

    m_ImGuiOverlay = new ImGuiLayer();
    m_LayerStack.pushOverlay(m_ImGuiOverlay);

    DM_CORE_INFO("Platform: {0}", DM_PLATFORM);

    initializeSubSystems();

    dm_dev = m_Window->GetDeviceManager();
    dev = dm_dev->GetDevice();
    tempInit();
    // m_ScriptManager.reloadGameLibrary("Assets/Scripts/build/libGameApp.so");
}

static float frameTime = 0;
void Application::runApplication()
{

    while (m_Running) {
        float frameStartTime = Time::getTime();
        Time::Update();

        m_Window->BeginFrame();

        EventSystem::ProcessEvents();

        tempUpdate();

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
        m_Window->EndFrame();

        frameTime = Time::getTime() - frameStartTime;
    }
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
