#ifndef VX_APPLICATIONH
#define VX_APPLICATIONH
#include <Core/Window.hpp>
#include <core.hpp>
namespace Voxler {

class Application {
public:
    Application(const std::string& title = "VoxlerEngine - Default", u32 width = 1280, u32 height = 720);

    void runApplication();

    inline Window& GetWindowVX() { return *m_Window; };

private:
    Scope<Window> m_Window;

    bool m_Running = true;

    float m_LastFrameTime = 0.0f;

    static bool m_ApplicationCreated;
};

}

#endif
