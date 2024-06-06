#ifndef VX_WINDOWH
#define VX_WINDOWH
#include <glad.h>
//
#include "GLFW/glfw3.h"
#include "imgui_impl_glfw.h"
#include <core.hpp>

namespace Voxler {

struct WindowSettings {

    u32 Width, Height;
    std::string Title;

    bool VSync = false;
};

class Window {
public:
    Window(const WindowSettings settings);
    ~Window() = default;

    void update();

    u32 getWidth() { return m_Settings.Height; }
    u32 getHeight() { return m_Settings.Width; }

    void setVsync(bool enabled);
    bool vsyncEnabled() { return m_Settings.VSync; }

    GLFWwindow* getGLFWWindow() { return m_Window; }

private:
    void initWindow(const WindowSettings& settings);
    void shutdown();

    void initCallbacks();

    GLFWwindow* m_Window;

    WindowSettings m_Settings;
};
}

#endif
