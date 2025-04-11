#ifndef DM_APPLICATIONH
#define DM_APPLICATIONH
#include <core.hpp>

#include "ImGui/ImGuiLayer.hpp"
#include "Rendering/Renderer.hpp"
#include "Scripting/NativeScriptManager.hpp"
#include "nvrhi/nvrhi.h"
#include <Core/LayerStack.hpp>
#include <Core/Window.hpp>

// #include <Rendering/FrameBuffer.hpp>

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
    NativeScriptManager& getScriptManager() { return m_ScriptManager; };

    static Ref<DeviceManager> getDeviceManager() { return getApp().getWindowDM().GetDeviceManager(); };
    static nvrhi::DeviceHandle getDevice() { return getApp().getWindowDM().GetDeviceManager()->GetDevice(); };

    ImGuiContext* getImGuiContext()
    {
        return ImGui::GetCurrentContext();
    }

    Ref<Scene> getSceneCTX();
    void setSceneCTX(Ref<Scene> scene) { m_SceneCTX = scene; };

    // static GraphicsAPI getGraphicsAPI() { return getApp().getWindowDM().getGraphicsAPI(); };

    // ---

private:
    void initializeSubSystems();

private:
    EventSystem m_EventSystem;
    NativeScriptManager m_ScriptManager;

private:
    Scope<Window> m_Window;

    Ref<Scene> m_SceneCTX = nullptr;

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
