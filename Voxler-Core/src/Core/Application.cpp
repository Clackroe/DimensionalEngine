#include <Core/Application.hpp>

namespace Voxler {
bool Application::m_ApplicationCreated = false;

Application::Application(const std::string& title, u32 width, u32 height)
{
    VX_CORE_ASSERT(!m_ApplicationCreated, "Application already created!! Aborting.");

    Log::Init();

    m_ApplicationCreated = true;
    m_Window = CreateScope<Window>((WindowSettings) { width, height, title });
}

void Application::runApplication()
{
    while (m_Running) {
        float time = (float)glfwGetTime();

        //------Update Layers-------
        for (Layer* layer : m_LayerStack) {
            layer->OnUpdate();
        }

        //------Update imgui Layers-------

        //------
        m_Window->update();
    }
}

}
