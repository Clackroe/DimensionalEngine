#ifndef DM_INPUTH
#define DM_INPUTH
#include <core.hpp>

#include <Event/EventSystem.hpp>
#include <Input/KeyCodes.hpp>

namespace Dimensional {

class DMCORE_API Input {
public:
    static Input& getInstance()
    {
        static Input s_Instance;
        return s_Instance;
    };

    static bool isKeyDown(KeyCode key);
    static bool isKeyReleased(KeyCode key);

    static float getMouseX();
    static float getMouseY();

    static void Init();

    void keyPressed(KeyCode key);
    void keyReleased(KeyCode key);

    void mouseMoved(float x, float y);

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

private:
    ~Input() = default;
    Input() = default;

    UMap<KeyCode, bool> m_KeyStates;

    float m_MouseX;
    float m_MouseY;
};

}

#endif
