#ifndef DM_SC_NATIVESCRIPTING
#define DM_SC_NATIVESCRIPTING
#include "EngineAPI.hpp"
#include "NativeScriptableEntity.hpp"
#include <CoreScriptLib.hpp>
#include <DefinesScriptLib.hpp>
#include <KeyCodes.hpp>
#include <ReflectionHelpers.hpp>

extern EngineAPI* g_EngineAPI;
extern ComponentAPI* g_CompAPI;

namespace Input {

inline bool isKeyDown(Dimensional::KeyCode key)
{
    return g_EngineAPI->Input_IsKeyDown(key);
}

inline bool isKeyReleased(Dimensional::KeyCode key)
{
    return g_EngineAPI->Input_IsKeyUp(key);
}

inline bool isMouseDown(Dimensional::MouseCode key)
{
    return g_EngineAPI->Input_IsMouseDown(key);
}

inline bool isMouseReleased(Dimensional::MouseCode key)
{
    return g_EngineAPI->Input_IsMouseUp(key);
}

inline float getMouseX()
{
    return g_EngineAPI->Input_GetMouseX();
}

inline float getMouseY()
{
    return g_EngineAPI->Input_GetMouseX();
}

}

namespace Log {
inline void Info(const char* msg)
{
    g_EngineAPI->LogInfo(msg);
}

inline void Warn(const char* msg)
{
    g_EngineAPI->LogWarn(msg);
}

inline void Error(const char* msg)
{
    g_EngineAPI->LogError(msg);
}
}

namespace Time {
}

namespace Componants {
}
#endif
