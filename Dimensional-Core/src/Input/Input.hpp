#ifndef VX_INPUTH
#define VX_INPUTH
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

    void Init();

private:
    static UMap<KeyCode, bool> s_KeyPressed;
    static UMap<KeyCode, bool> s_KeyReleased;

    static Input* s_Instance;
};

static Input* s_Instance = nullptr;

}

#endif
