#include "ImGui/ImGuiLayer.hpp"
#include <Core/Application.hpp>

namespace Voxler {

Application* Application::s_Application = nullptr;

Application::Application(const std::string& title, u32 width, u32 height)
{

    VX_CORE_ASSERT(!s_Application, "Application already created!! Aborting.");
    s_Application = this;

    Log::Init();

    m_Window = CreateScope<Window>((WindowSettings) { width, height, title });

    m_ImGuiOverlay = new ImGuiLayer();
    m_LayerStack.pushOverlay(m_ImGuiOverlay);
}

void Application::runApplication()
{
    while (m_Running) {

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

}
