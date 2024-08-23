#ifndef DM_INPUTH
#define DM_INPUTH
#include <core.hpp>

#include <Event/EventSystem.hpp>
#include <Input/KeyCodes.hpp>

namespace Dimensional {

class DMCORE_API Input {
public:
    Input() = default;
    ~Input() = default;

    static bool IsKeyDown(KeyCode key);
    static bool IsKeyReleased(KeyCode key);

    static float getMouseX() { return m_MouseX; };
    static float getMouseY() { return m_MouseY; };

    void Init();

private:
    static Input* s_Instance;

    static float m_MouseX;
    static float m_MouseY;
};

static Input* s_Instance = nullptr;

}

#endif
