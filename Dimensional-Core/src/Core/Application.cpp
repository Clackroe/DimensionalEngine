#include "EngineAPI.hpp"
#include "GLFW/glfw3.h"
#include "ImGui/ImGuiLayer.hpp"
#include "Log/log.hpp"
#include "Rendering/GPUBuffer.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ShaderCompiler.hpp"
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

nvrhi::TextureHandle textureTest1;

nvrhi::SamplerHandle sampler;

ShaderVarient vs;
ShaderVarient ps;

nvrhi::FramebufferHandle fb;

nvrhi::BindingLayoutHandle layout;

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec2 uv2;
};

static const Vertex g_Vertices[] = {
    //  position
    { { 0.f, 0.5f, 0.f }, { 0.5f, 1.0f } },
    { { 0.5f, -0.5f, 0.f }, { 1.0f, 0.0f } },
    { { -0.5f, -0.5f, 0.f }, { 0.0f, 0.0f } },
};

static const Vertex g_Vertices1[] = {
    //  position
    { { 0.f, -0.5f, 0.f } },
    { { 0.5f, 0.5f, 0.f } },
    { { -0.5f, 0.5f, 0.f } },
};

nvrhi::BufferHandle vertexBuffer;
nvrhi::BufferHandle vertexBuffer1;
nvrhi::GraphicsPipelineHandle graphicsPipeline;
nvrhi::BindingSetHandle bindingSet;
unsigned char* imageBytes;
int w, h, c;

static void tempInit()
{
    imageBytes = stbi_load("Assets/Textures/Albedo.png", &w, &h, &c, STBI_rgb_alpha);

    cmd = dev->createCommandList();

    ShaderCompiler comp;
    ShaderCompileOptions opts;
    opts.optimizationLevel = 0;
    opts.includePaths = { "Assets/Shaders" };

    ShaderCreateInfo info;
    info.includePaths = { "Assets/Shaders" };
    info.optimizationLevel = 0;
    shader = Shader::Create("Assets/Shaders/helloworld.slang", info);

    nvrhi::TextureDesc td;
    td.debugName = "Att 1";
    td.setFormat(nvrhi::Format::RGBA8_UNORM);
    td.setWidth(w);
    td.setHeight(h);

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
                               .addColorAttachment(textureTest1)
                               .addColorAttachment(textureTest2);

    auto samDesc = nvrhi::SamplerDesc();
    sampler = dev->createSampler(samDesc);

    nvrhi::FramebufferHandle framebuffer = dev->createFramebuffer(framebufferDesc);
    if (!framebuffer) {
        DM_CORE_ERROR("Failed to create framebuff")
    }

    nvrhi::BindingLayoutDesc lDesc;
    lDesc.setRegisterSpaceIsDescriptorSet(true);
    lDesc.setRegisterSpace((u32)RESOURCE_DOMAIN::PIPELINE);
    lDesc.visibility = nvrhi::ShaderType::AllGraphics;

    auto item = nvrhi::BindingLayoutItem::Texture_SRV(0);
    lDesc.addItem(item);
    item = nvrhi::BindingLayoutItem::Sampler(1);
    lDesc.addItem(item);

    layout = dev->createBindingLayout(lDesc);

    auto pipelineDesc = nvrhi::GraphicsPipelineDesc()
                            .setInputLayout(shader->GetShaderVariant(nvrhi::ShaderType::Vertex).inputLayout)
                            .setVertexShader(shader->GetShaderHandle(nvrhi::ShaderType::Vertex))
                            .setPixelShader(shader->GetShaderHandle(nvrhi::ShaderType::Pixel))
                            .addBindingLayout(layout);
    // for (auto& b : shader->GetBindingLayouts()) {
    //     pipelineDesc.addBindingLayout(b);
    // }

    nvrhi::VulkanBindingOffsets a;
    pipelineDesc.primType = nvrhi::PrimitiveType::TriangleList;
    pipelineDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
    pipelineDesc.renderState.depthStencilState.depthTestEnable = false;

    auto f = Application::getDeviceManager()->GetCurrentFramebuffer();
    graphicsPipeline = dev->createGraphicsPipeline(pipelineDesc, f);
    if (!graphicsPipeline) {
        DM_CORE_ERROR("Failed to create Pipeline")
    }

    auto vertexBufferDesc = nvrhi::BufferDesc()
                                .setByteSize(sizeof(g_Vertices))
                                .setIsVertexBuffer(true)
                                .setInitialState(nvrhi::ResourceStates::VertexBuffer)
                                .setKeepInitialState(true) // enable fully automatic state tracking
                                .setDebugName("Vertex Buffer");

    vertexBuffer = dev->createBuffer(vertexBufferDesc);
    vertexBuffer1 = dev->createBuffer(vertexBufferDesc);

    auto bindingSetDesc = nvrhi::BindingSetDesc().addItem(nvrhi::BindingSetItem::Texture_SRV(0, textureTest1)).addItem(nvrhi::BindingSetItem::Sampler(1, sampler));

    bindingSet = dev->createBindingSet(bindingSetDesc, layout);
};

static void tempUpdate()
{

    cmd->open();
    cmd->writeTexture(textureTest1, 0, 0, imageBytes, w * 4);
    cmd->writeBuffer(vertexBuffer, g_Vertices, sizeof(g_Vertices));
    cmd->writeBuffer(vertexBuffer1, g_Vertices1, sizeof(g_Vertices));
    cmd->close();
    dev->executeCommandList(cmd);
    cmd->open();

    fb = dm_dev->GetCurrentFramebuffer();
    nvrhi::utils::ClearColorAttachment(cmd, fb, 0, { static_cast<float>(std::sin(glfwGetTime())), static_cast<float>(std::sin(glfwGetTime())), 0.5, 1 });

    nvrhi::VertexBufferBinding t;
    t.setSlot(0);
    t.setBuffer(vertexBuffer);
    t.setOffset(0);

    nvrhi::VertexBufferBinding t1;
    t1.setSlot(1);
    t1.setBuffer(vertexBuffer1);
    t1.setOffset(0);

    auto graphicsState = nvrhi::GraphicsState()
                             .setPipeline(graphicsPipeline)
                             .setFramebuffer(fb)
                             .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(Application::getApp().getWindowDM().getWidth(), Application::getApp().getWindowDM().getHeight())))
                             .addBindingSet(bindingSet)
                             .addVertexBuffer(t)
                             .addVertexBuffer(t1);

    cmd->setGraphicsState(graphicsState);
    // cmd->setResourceStatesForBindingSet();

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
