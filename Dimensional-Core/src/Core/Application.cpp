#include "ImGui/ImGuiLayer.hpp"
#include "Log/log.hpp"
#include "Rendering/Renderer.hpp"
#include <Core/Application.hpp>
#include <buffer.hpp>

#include <Core/Time.hpp>

#include <Core/EditorCamera.hpp>

#include <glad.h>

namespace Dimensional {

Application* Application::s_Application = nullptr;

Application::Application(const std::string& title, u32 width, u32 height)
{
    initializeSubSystems();

    std::replace(fPath.begin(), fPath.end(), '\\', '/');
    engineAssetDirectory = fPath.substr(0, fPath.find_last_of('/')) + "Assets";

    DM_CORE_INFO("PATH: {0}", fPath.c_str())
    DM_CORE_ASSERT(!s_Application, "Application already created!! Aborting.");
    s_Application = this;

    m_Window = CreateScope<Window>(WindowSettings { width, height, title });

    m_ImGuiOverlay = new ImGuiLayer();
    m_LayerStack.pushOverlay(m_ImGuiOverlay);

    DM_CORE_INFO("Platform: {0}", DM_PLATFORM);
}

void Application::runApplication()
{
    while (m_Running) {
        Time::Update();

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
    m_Renderer.Init();
}

}
