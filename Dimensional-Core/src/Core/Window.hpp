#ifndef DM_WINDOWH
#define DM_WINDOWH

#include "Rendering/DeviceManager.hpp"
#include "Rendering/Vulkan/VulkanDevice.hpp"
#include "nvrhi/nvrhi.h"
#include <core.hpp>

class GLFWwindow;

typedef void* (*GLADloadproc)(const char* name);

namespace Dimensional {

struct WindowSettings {

    u32 Width, Height;
    std::string Title;
    bool VSync = false;
};

class Window {
public:
    Window(const WindowSettings settings);
    ~Window() = default;

    void BeginFrame();

    void EndFrame();

    u32 getWidth() { return m_Settings.Width; }
    u32 getHeight() { return m_Settings.Height; }

    void setVsync(bool enabled);
    bool vsyncEnabled() { return m_Settings.VSync; }

    GLFWwindow* getGLFWWindow() { return m_Window; }

    // TEMP
    nvrhi::CommandListHandle CommandList;

private:
    void initWindow(const WindowSettings& settings);
    void shutdown();

    void initCallbacks();

    // TEMPRORARY
    Ref<DeviceManager> device;

    GLFWwindow* m_Window;

    WindowSettings m_Settings;
};
}

#endif
