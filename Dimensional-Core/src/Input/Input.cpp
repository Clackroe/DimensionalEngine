#include <Input/Input.hpp>

namespace Dimensional {

UMap<KeyCode, bool> Input::s_KeyPressed;
UMap<KeyCode, bool> Input::s_KeyReleased;

Input* Input::s_Instance = nullptr;

bool Input::IsKeyDown(KeyCode key) { return s_KeyPressed[key]; };
bool Input::IsKeyReleased(KeyCode key) { return s_KeyReleased[key]; };

void Input::Init()
{
    if (s_Instance != nullptr) {
        DM_CORE_WARN("Input System already Initialized!!");
    }

    s_Instance = this;

    EventSystem::AddListener<KeyEvent>([](const Ref<KeyEvent>& e) {
        Mode mode = e->getMode();
        KeyCode key = e->getKey();

        if (mode == Key::RELEASE) {
            s_KeyReleased[key] = true;
            s_KeyPressed[key] = false;
        } else if (mode == Key::PRESS) {
            s_KeyReleased[key] = false;
            s_KeyPressed[key] = true;
        }
    });

    DM_CORE_INFO("Input System Initialized");
}
}
