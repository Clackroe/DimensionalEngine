#include <voxler.hpp>

namespace Voxler {

// class TestLayer : public Layer {
//
//     virtual void OnAttatch() override
//     {
//         EventSystem::AddListener<KeyEvent>(*[](const Ref<KeyEvent>& e) {
//             VX_INFO("Event Received: {0}", e->getKey());
//         });
//         EventSystem::RaiseEvent<KeyEvent>(32);
//     }
//     virtual void OnDetatch() override { }
//     virtual void OnUpdate() override { }
//     virtual void OnImGuiRender() override { }
// };

class TestBed : public Application {
public:
    TestBed()
    {
        VX_INFO("Hello From Testbed");
        EventSystem::AddListener<KeyEvent>(*[](const Ref<KeyEvent>& e) {
            VX_INFO("Event Received: {0}", e->getKey());
        });
        EventSystem::RaiseEvent<KeyEvent>(32);
        EventSystem::RaiseEvent<KeyEvent>(66);
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
