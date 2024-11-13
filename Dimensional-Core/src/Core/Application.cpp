#include "ImGui/ImGuiLayer.hpp"
#include "Log/log.hpp"
#include "Scripting/NativeScriptManager.hpp"
#include "core.hpp"
#include <Core/Application.hpp>

#include <Core/Time.hpp>

namespace Dimensional {

Application* Application::s_Application = nullptr;

Application::Application(const std::string& title, u32 width, u32 height)
{
    Log::Init(); // Logging needs to be initialized sooner than the other systems
    //
    DM_CORE_INFO("TEST");

    DM_CORE_ASSERT(!s_Application, "Application already created!! Aborting.");
    s_Application = this;

    m_Window = CreateScope<Window>(WindowSettings { width, height, title });
    m_ImGuiOverlay = new ImGuiLayer();
    m_LayerStack.pushOverlay(m_ImGuiOverlay);

    DM_CORE_INFO("Platform: {0}", DM_PLATFORM);

    initializeSubSystems();
}

static float frameTime = 0;

void Application::runApplication()
{

    NativeScriptManager managerTemp;

    managerTemp.reloadGameLibrary("Assets/libGameApp.so");

    while (m_Running) {
        float frameStartTime = Time::getTime();
        Time::Update();

        EventSystem::ProcessEvents();

        //------Update Layers-------
        for (Layer* layer : m_LayerStack) {
            layer->OnUpdate();
        }

        //------Update imgui Layers-------
        m_ImGuiOverlay->beginFrame();

        // ImGui::Begin("Stats");
        // ImGui::Text("FPS: %f", 1 / Time::deltaTime());
        // ImGui::End();

        for (Layer* layer : m_LayerStack) {
            layer->OnImGuiRender();
        }

        m_ImGuiOverlay->endFrame();

        //------
        m_Window->update();
        frameTime = Time::getTime() - frameStartTime;
    }
}

void Application::initializeSubSystems()
{
    m_EventSystem.Init();
    Input::Init();
    m_Renderer.Init(m_Window->getLoadProc());
}
}
