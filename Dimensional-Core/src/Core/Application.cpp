#include "ImGui/ImGuiLayer.hpp"
#include <Core/Application.hpp>

namespace Dimensional {

Application* Application::s_Application = nullptr;

Application::Application(const std::string& title, u32 width, u32 height)
{

    VX_CORE_ASSERT(!s_Application, "Application already created!! Aborting.");
    s_Application = this;

    initializeSubSystems();

    m_Window = CreateScope<Window>((WindowSettings) { width, height, title });

    m_ImGuiOverlay = new ImGuiLayer();
    m_LayerStack.pushOverlay(m_ImGuiOverlay);

    VX_CORE_INFO("Platform: {0}", VX_PLATFORM);
}

void Application::runApplication()
{
    while (m_Running) {

        EventSystem::ProcessEvents();

        //------Update Layers-------
        for (Layer* layer : m_LayerStack) {
            layer->OnUpdate();
        }

        //------Update imgui Layers-------
        m_ImGuiOverlay->beginFrame();

        for (Layer* layer : m_LayerStack) {
            layer->OnImGuiRender();
        }

        m_ImGuiOverlay->endFrame();
        //------
        m_Window->update();
    }
}

void Application::initializeSubSystems()
{
    Log::Init();
    m_EventSystem.Init();
    m_Input.Init();
}

}
