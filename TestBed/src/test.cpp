#include <voxler.hpp>

namespace Dimensional {

class TestLayer : public Layer {

    virtual void OnAttatch() override
    {
        VX_INFO("TestLayer Attached!!");
    }
    virtual void OnDetatch() override { }
    virtual void OnUpdate() override
    {

        if (Input::IsKeyDown(Key::Escape)) {
            Application::getApp().stopApplication();
        }
    }
    virtual void OnImGuiRender() override { }
};

class TestBed : public Application {
public:
    TestBed()
    {
        PushLayer(new TestLayer());
    }

    ~TestBed()
    {
    }
};

Application* EXT_InitApplication()
{
    return new TestBed();
}
}
