#ifndef DM_EVENTH
#define DM_EVENTH
#include <KeyCodes.hpp>
#include <core.hpp>

namespace Dimensional {

class DMCORE_API Event {
public:
    virtual ~Event() = default;
    virtual const char* getName() const = 0;
};

class KeyEvent : public Event {
public:
    KeyEvent(KeyCode code, Mode mode)
        : m_KeyCode(code)
        , m_Mode(mode)
    {
    }
    u32 getKey() const { return m_KeyCode; }
    Mode getMode() const { return m_Mode; }
    const char* getName() const override { return "KeyEvent"; }

private:
    KeyCode m_KeyCode;
    Mode m_Mode;
};

// Mouse Events

class MouseEvent : public Event {
public:
    MouseEvent(u32 x, u32 y)
        : m_X(x)
        , m_Y(y)
    {
    }
    u32 getX() const { return m_X; }
    u32 getY() const { return m_Y; }
    const char* getName() const override { return "MouseEvent"; }

private:
    u32 m_X, m_Y;
};

// Window Events

}
#endif
