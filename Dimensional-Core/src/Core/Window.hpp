#ifndef DM_WINDOWH
#define DM_WINDOWH

// #include "Rendering/DeviceManager.hpp"
#include "Rendering/Vulkan/VulkanDevice.hpp"
#include "nvrhi/nvrhi.h"
#include <core.hpp>

class GLFWwindow;

typedef void* (*GLADloadproc)(const char* name);

namespace Dimensional {

struct WindowSettings {

    u32 Width, Height;
    std::string Title;
    bool startFullScreen = false, startBorderless = false;
    u32 refreshRate = 0;
    nvrhi::Format format = nvrhi::Format::RGBA8_UNORM;
    bool VSync = false;
    bool resizeable = true;
    bool visible = true;

    u32 swapChainSamples = 1;
};

class Window {
public:
    static void Initialize();
    static void Shutdown();

    static Ref<Window> Create(const WindowSettings settings);
    ~Window();

    void PollEvents();

    void EndFrame();

    u32 GetWidth() { return m_Settings.Width; }
    u32 GetHeight() { return m_Settings.Height; }

    const WindowSettings& GetSettings() const { return m_Settings; }

    bool IsVisible() const;
    bool IsInFocus() const;
    bool IsIconified() const;
    bool ShouldClose() const;

    void SetVSync(bool enabled);
    bool IsVSyncEnabled() const { return m_Settings.VSync; }

    void SetSize(u32 width, u32 height);
    void SetTitle(const std::string& title);
    void SetFullscreen(bool fullscreen);
    void SetBorderless(bool borderless);
    void SetVisible(bool visible);

    void Close();

    // Monitor/Display info
    void GetFramebufferSize(int& width, int& height) const;
    void GetMonitorInfo(int& width, int& height, int& refreshRate) const;

    GLFWwindow* getGLFWWindow() { return m_Window; }
    const char** GetRequiredExtensions_VK(u32& count);

    void* CreateSurface_VK(void* vkInstance, void* surface);

private:
    Window() = default;

    bool CreateWindow();

    void SetFormatHints(nvrhi::Format format);

    void ApplyWindowHints();

    void initCallbacks();

    GLFWwindow* m_Window = nullptr;

    WindowSettings m_Settings;
};
}

#endif
