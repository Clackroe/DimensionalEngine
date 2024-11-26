#include <NativeScripting.hpp>

namespace ScriptingCore {

std::vector<std::function<void(NativeScriptRegistry*)>> ScriptCoreLink::s_RegistrationFunctions;

ScriptCoreLink* ScriptCoreLink::s_Instance = nullptr;

void ScriptCoreLink::Init(EngineAPI* eAPI, ComponentAPI* cAPI)
{
    s_Instance = new ScriptCoreLink(eAPI, cAPI);
}

void ScriptCoreLink::ShutDown()
{
    getInstance()->m_ComponentAPI = nullptr;
    getInstance()->m_EngineAPI = nullptr;
    s_RegistrationFunctions.clear();
    delete s_Instance;
    s_Instance = nullptr;
}

ScriptCoreLink* ScriptCoreLink::getInstance()
{
    // if (s_Instance == nullptr) {
    //     std::cout << "TRIED TO GET NULL SCRIPTE CORE LINK INSTANCE" << std::endl;
    //     return nullptr;
    // }
    return s_Instance;
};

ScriptCoreLink::ScriptCoreLink(EngineAPI* eAPI, ComponentAPI* cAPI)
{
    m_EngineAPI = eAPI;
    m_ComponentAPI = cAPI;
}
}
