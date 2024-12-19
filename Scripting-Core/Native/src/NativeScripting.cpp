#include <NativeScripting.hpp>

namespace ScriptingCore {

std::vector<std::function<void(NativeScriptRegistry*)>>& ScriptCoreLink::getRegFuncs()
{
    static std::vector<std::function<void(NativeScriptRegistry*)>> s_RegistrationFunctions;
    return s_RegistrationFunctions;
}

ScriptCoreLink* ScriptCoreLink::s_Instance = nullptr;

void ScriptCoreLink::Init(EngineAPI* eAPI, ComponentAPI* cAPI)
{
    s_Instance = new ScriptCoreLink(eAPI, cAPI);
}

void ScriptCoreLink::ShutDown()
{
    getInstance()->m_ComponentAPI = nullptr;
    getInstance()->m_EngineAPI = nullptr;
    std::cout << "Shutting down script core link " << std::endl;
    getRegFuncs().clear();
    delete s_Instance;
    s_Instance = nullptr;
}

ScriptCoreLink* ScriptCoreLink::getInstance()
{
    return s_Instance;
};

ScriptCoreLink::ScriptCoreLink(EngineAPI* eAPI, ComponentAPI* cAPI)
{
    m_EngineAPI = eAPI;
    m_ComponentAPI = cAPI;
}
}
