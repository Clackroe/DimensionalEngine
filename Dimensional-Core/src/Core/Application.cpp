#include "Core/Window.hpp"
#include "EngineAPI.hpp"
#include "GLFW/glfw3.h"
#include "ImGui/ImGuiLayer.hpp"
#include "Log/log.hpp"
#include "Rendering/Buffer.hpp"
#include "Rendering/GPUBuffer.hpp"

#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderDevice.hpp"
#include "Rendering/RenderTarget.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ShaderCompiler.hpp"
#include "Scripting/NativeScriptManager.hpp"
#include "core.hpp"
#include "nvrhi/nvrhi.h"
#include "nvrhi/utils.h"
#include <Core/Application.hpp>
#include <imgui_impl_vulkan.h>

#include <Rendering/Texture.hpp>

#include <stb_image.hpp>

#include <Core/Time.hpp>

namespace Dimensional {

Application* Application::s_Application = nullptr;

nvrhi::CommandListHandle cmd;
Ref<RenderDevice> dm_dev;
nvrhi::IDevice* dev;

Ref<Shader> shader;
Ref<Texture2D> tex;

Ref<RenderTarget> rTarget;

Ref<GraphicsPipeline> pipe;

nvrhi::FramebufferHandle fb;

nvrhi::BindingLayoutHandle layout;

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec2 uv2;
};

struct ColTest {
    glm::vec4 col = { 5, 2, 1, 1.0 };
} test1;

static const Vertex g_Vertices[] = {
    { { 0.f, 0.5f, 0.f }, { 1.5f, 3.0f } },
    { { 0.5f, -0.5f, 0.f }, { 3.0f, 0.0f } },
    { { -0.5f, -0.5f, 0.f }, { 0.0f, 0.0f } },
};

Ref<VertexBuffer> vBuff;
// nvrhi::BufferHandle vertexBuffer;
unsigned char* imageBytes;

Ref<ConstantBuffer> cBuf;

int w, h, c;

static void tempInit()
{

    BufferCreateInfo in;
    in.debugName = "Color Test";
    in.sizeBytes = sizeof(ColTest);
    cBuf = ConstantBuffer::Create(in);
    cmd = dev->createCommandList();

    cBuf->SetData(&test1, sizeof(ColTest));

    imageBytes = stbi_load("Assets/Resources/Folder.png", &w, &h, &c, 0);

    ShaderCreateInfo info;
    info.includePaths = { "Assets/Shaders" };
    info.optimizationLevel = 0;
    shader = Shader::Create("Assets/Shaders/helloworld.slang", info);

    TextureCreateInfo td;
    td.width = w;
    td.height = h;
    td.format = TextureFormat::RGBA;
    td.debugName = "A FRICKING TEXTURE";
    tex = Texture2D::Create(td);
    tex->SetData(imageBytes, w * h * c * 4);

    td.debugName += "2";
    Ref<Texture2D> t2 = Texture2D::Create(td);
    //
    // auto framebufferDesc = nvrhi::FramebufferDesc()
    //                            .addColorAttachment(tex->GetHandle())
    //                            .addColorAttachment(t2->GetHandle());
    //
    // nvrhi::FramebufferHandle framebuffer = dev->createFramebuffer(framebufferDesc);
    // if (!framebuffer) {
    //     DM_CORE_ERROR("Failed to create framebuff")
    // }

    RenderTargetCreateInfo info1;
    info1.clearColor = true;
    info1.clearColorValue = glm::vec4(1.0, 0.5, 0.1, 1.0);
    info1.width = 1920;
    info1.height = 1080;

    info1.colorAttachments = { { TextureFormat::RGBA, false }, { TextureFormat::RGBA, true } };

    rTarget = RenderTarget::Create(info1);

    GraphicsPipelineCreateinfo i;
    i.debugName = "Test Pipeline";
    i.shader = shader;
    i.renderTarget = rTarget;
    pipe = GraphicsPipeline::Create(i);
    pipe->SetRendererConstantSpace(Renderer::GetConstantBindingSet());
    pipe->SetTexture(tex, 0);
    pipe->SetConstantBuffer(cBuf, 1);
    pipe->Compile();

    // pipelineDesc.primType = nvrhi::PrimitiveType::TriangleList;
    //
    // pipelineDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
    // pipelineDesc.renderState.depthStencilState.depthTestEnable = false;

    BufferCreateInfo t;
    t.sizeBytes = sizeof(g_Vertices);
    vBuff = VertexBuffer::Create(t);
    vBuff->SetData(g_Vertices, sizeof(g_Vertices));
};

static void tempUpdate()
{

    cmd->open();

    fb = dm_dev->GetCurrentFramebuffer();
    nvrhi::utils::ClearColorAttachment(cmd, fb, 0, { static_cast<float>(std::sin(glfwGetTime())), static_cast<float>(std::sin(glfwGetTime())), 0.5, 1 });

    auto graphicsState = nvrhi::GraphicsState();

    graphicsState.addVertexBuffer(vBuff->GetBinding());

    pipe->Bind(cmd, graphicsState);

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

    WindowSettings settings;
    settings.Width = width;
    settings.Height = height;
    settings.Title = title;
    settings.VSync = false;

    m_Window = Window::Create(settings);
    DM_CORE_INFO("Window Created")

    // m_Window = CreateScope<Window>(WindowSettings { width, height, title });
    m_Device = RenderDevice::Create(nvrhi::GraphicsAPI::VULKAN);
    RenderDeviceCreateInfo info;
    info.enableDebugLayer = true;
    info.enableValidationLayer = true;
    info.swapChainFormat = settings.format;
    info.api = nvrhi::GraphicsAPI::VULKAN;
    m_Device->InitializeDevice(m_Window, info);

    m_ImGuiOverlay = new ImGuiLayer();
    m_LayerStack.pushOverlay(m_ImGuiOverlay);

    DM_CORE_INFO("Platform: {0}", DM_PLATFORM);

    initializeSubSystems();

    dm_dev = m_Device;
    dev = m_Device->GetDevice();
    tempInit();
    // m_ScriptManager.reloadGameLibrary("Assets/Scripts/build/libGameApp.so");
}

static float frameTime = 0;
void Application::runApplication()
{

    while (m_Running) {
        float frameStartTime = Time::getTime();
        Time::Update();

        m_Window->PollEvents();
        m_Device->BeginFrame();

        EventSystem::ProcessEvents();

        tempUpdate();

        //------Update Layers-------
        for (Layer* layer : m_LayerStack) {
            layer->OnUpdate();
        }

        //------Update imgui Layers-------
        // m_ImGuiOverlay->beginFrame();
        //
        // ImGui::Begin("Stats");
        // ImGui::Text("FPS: %f", 1 / Time::deltaTime());
        // ImGui::End();
        //
        // for (Layer* layer : m_LayerStack) {
        //     layer->OnImGuiRender();
        // }
        //
        // m_ImGuiOverlay->endFrame();

        //------
        m_Device->Present();
        m_Window->EndFrame();

        frameTime = Time::getTime() - frameStartTime;
    }
}

void Application::initializeSubSystems()
{
    Renderer::Init({});
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
