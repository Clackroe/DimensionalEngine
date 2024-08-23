#include <dimensional.hpp>

#include <Entry.hpp>
namespace Dimensional {

class TestLayer : public Layer {

    virtual void OnAttatch() override
    {
        DM_INFO("TestLayer Attached!!");
        //DM_INFO("TEST {0}", glm::vec3(0.0f, 0.0f, 0.0f));
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
