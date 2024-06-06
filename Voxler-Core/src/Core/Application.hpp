#ifndef VX_APPLICATIONH
#define VX_APPLICATIONH
#include "ImGui/ImGuiLayer.hpp"
#include <Core/LayerStack.hpp>
#include <Core/Window.hpp>
#include <core.hpp>

// Using Singleton Pattern | There should only ever be one application.
namespace Voxler {

class Application {
public:
    Application(const std::string& title = "VoxlerEngine - Default", u32 width = 1280, u32 height = 720);

    void runApplication();

    static Application& getApp() { return *s_Application; }
    inline Window& getWindowVX() { return *m_Window; };

private:
    Scope<Window> m_Window;

    LayerStack m_LayerStack;

    ImGuiLayer* m_ImGuiOverlay;

    bool m_Running = true;

    float m_LastFrameTime = 0.0f;

    static Application* s_Application;
};

}

#endif
