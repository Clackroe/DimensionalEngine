#ifndef DM_WINDOWH
#define DM_WINDOWH
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

    void update();

    u32 getWidth() { return m_Settings.Height; }
    u32 getHeight() { return m_Settings.Width; }

    void setVsync(bool enabled);
    bool vsyncEnabled() { return m_Settings.VSync; }

    GLFWwindow* getGLFWWindow() { return m_Window; }

    GLADloadproc getLoadProc() { return m_GLLoadProc; };

private:
    void initWindow(const WindowSettings& settings);
    void shutdown();

    void initCallbacks();

    GLFWwindow* m_Window;

    GLADloadproc m_GLLoadProc;

    WindowSettings m_Settings;
};
}

#endif
