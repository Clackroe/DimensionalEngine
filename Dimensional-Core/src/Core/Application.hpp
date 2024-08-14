#ifndef DM_APPLICATIONH
#define DM_APPLICATIONH
#include <core.hpp>

#include "ImGui/ImGuiLayer.hpp"
#include <Core/LayerStack.hpp>
#include <Core/Window.hpp>

#include <Event/EventSystem.hpp>

#include <Input/Input.hpp>

// Using Singleton Pattern | There should only ever be one application. namespace Dimensional {
namespace Dimensional {

class DMCORE_API Application {
public:
    Application(const std::string& title = "DimensionalEngine - Default", u32 width = 1280, u32 height = 720);

    void runApplication();
    void stopApplication() { m_Running = false; };

    void PushLayer(Layer* layer) { m_LayerStack.pushLayer(layer); }

    static Application& getApp() { return *s_Application; }
    inline Window& getWindowDM() { return *m_Window; };

private:
    void initializeSubSystems();

private:
    EventSystem m_EventSystem;
    Input m_Input;

private:
    Scope<Window> m_Window;

    LayerStack m_LayerStack;

    ImGuiLayer* m_ImGuiOverlay;

    bool m_Running = true;

    float m_LastFrameTime = 0.0f;

    static Application* s_Application;
};

// Defined in client to intialize the app
Application* EXT_InitApplication();
}

#endif
