#include "Core/Application.hpp"
#include "KeyCodes.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/VAO.hpp"
#include <PortalEditor.hpp>
#include <dimensional.hpp>

#include <Entry.hpp>
namespace Dimensional {

class TestLayer : public Layer {
public:
    TestLayer() = default;
    virtual void OnDetatch() override {};

    virtual void OnAttatch() override {
    };
    virtual void OnUpdate() override
    {
        if (Input::isKeyDown(Key::Escape)) {
            Application::getApp().stopApplication();
        }
    };
    virtual void OnImGuiRender() override {};
};

class PortalApp : public Application {
public:
    PortalApp()
        : Application("Portal Editor", 1920, 1080)
    {
        PushLayer(new TestLayer());
    }

    ~PortalApp()
    {
    }
};

Application* EXT_InitApplication()
{
    Application* app = new PortalApp();
    // ImGui::SetCurrentContext(app->getImGuiContext());
    return app;
}
}
