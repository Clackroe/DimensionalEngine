#ifndef DM_SC_NATIVESCRIPTING
#define DM_SC_NATIVESCRIPTING
#include "EngineAPI.hpp"
#include <CoreScriptLib.hpp>
#include <DefinesScriptLib.hpp>
#include <KeyCodes.hpp>
#include <functional>

namespace ScriptingCore {

class ScriptCoreLink {
public:
    static void Init(EngineAPI* eAPI, ComponentAPI* cAPI);
    static void ShutDown();

    static ScriptCoreLink* getInstance();

    static EngineAPI* getEngineAPI() { return getInstance()->_getEngineAPI(); }
    static ComponentAPI* getComponentAPI() { return getInstance()->_getComponentAPI(); }

    ScriptCoreLink(EngineAPI* eAPI, ComponentAPI* cAPI);
    ~ScriptCoreLink() = default;

private:
    EngineAPI* _getEngineAPI() { return m_EngineAPI; }
    ComponentAPI* _getComponentAPI() { return m_ComponentAPI; }

    static ScriptCoreLink* s_Instance;

    EngineAPI* m_EngineAPI = nullptr;
    ComponentAPI* m_ComponentAPI = nullptr;
};

namespace Input {

    inline bool isKeyDown(Dimensional::KeyCode key)
    {
        return ScriptCoreLink::getEngineAPI()->Input_IsKeyDown(key);
    }

    inline bool isKeyReleased(Dimensional::KeyCode key)
    {
        return ScriptCoreLink::getEngineAPI()->Input_IsKeyUp(key);
    }

    inline bool isMouseDown(Dimensional::MouseCode key)
    {
        return ScriptCoreLink::getEngineAPI()->Input_IsMouseDown(key);
    }

    inline bool isMouseReleased(Dimensional::MouseCode key)
    {
        return ScriptCoreLink::getEngineAPI()->Input_IsMouseUp(key);
    }

    inline float getMouseX()
    {
        return ScriptCoreLink::getEngineAPI()->Input_GetMouseX();
    }

    inline float getMouseY()
    {
        return ScriptCoreLink::getEngineAPI()->Input_GetMouseX();
    }

}

namespace Log {
    inline void Info(const char* msg)
    {
        ScriptCoreLink::getEngineAPI()->LogInfo(msg);
    }

    inline void Warn(const char* msg)
    {
        ScriptCoreLink::getEngineAPI()->LogWarn(msg);
    }

    inline void Error(const char* msg)
    {
        ScriptCoreLink::getEngineAPI()->LogError(msg);
    }
}

namespace Time {
}

namespace Componants {
}
}
#endif
