#ifndef DM_INPUTH
#define DM_INPUTH
#include <core.hpp>

#include <Event/EventSystem.hpp>
#include <Input/KeyCodes.hpp>

namespace Dimensional {

class Input {
public:
    Input() = default;
    ~Input() = default;

    static bool IsKeyDown(KeyCode key);
    static bool IsKeyReleased(KeyCode key);

    static float getMouseX() { return m_MouseX; };
    static float getMouseY() { return m_MouseY; };

    void Init();

private:
    static UMap<KeyCode, bool> s_KeyPressed;
    static UMap<KeyCode, bool> s_KeyReleased;

    static Input* s_Instance;

    static float m_MouseX;
    static float m_MouseY;
};

static Input* s_Instance = nullptr;

}

#endif
