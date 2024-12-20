#include <NativeScripting.hpp>
#include <iostream>

namespace ScriptingCore {

ScriptCoreLink* ScriptCoreLink::s_Instance = nullptr;

std::vector<std::function<void(NativeScriptRegistry*)>> ScriptCoreLink::s_RegistrationFunctions;

void ScriptCoreLink::Init(EngineAPI* eAPI, ComponentAPI* cAPI)
{
    s_Instance = new ScriptCoreLink(eAPI, cAPI);
}

void ScriptCoreLink::ShutDown()
{
    getInstance()->m_ComponentAPI = nullptr;
    getInstance()->m_EngineAPI = nullptr;
    std::cout << "Shutting down script core link " << std::endl;
    s_RegistrationFunctions.clear();
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
