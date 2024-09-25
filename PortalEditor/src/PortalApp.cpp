#include <PortalEditor.hpp>
#include <dimensional.hpp>





#include <Entry.hpp>
namespace Dimensional {

class PortalApp : public Application {
public:
    PortalApp()
        : Application("Portal Editor", 1920, 1080)
    {
        PushLayer(new PortalLayer());
    }

    ~PortalApp()
    {
    }
};

Application* EXT_InitApplication()
{
    Application* app = new PortalApp();
    ImGui::SetCurrentContext(app->getImGuiContext());
    return app;
}
}
