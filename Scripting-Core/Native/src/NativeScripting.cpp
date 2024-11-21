#include <NativeScripting.hpp>

namespace ScriptingCore {

std::vector<std::function<void(NativeScriptRegistry*)>> ScriptCoreLink::s_RegistrationFunctions;

std::unique_ptr<ScriptCoreLink> ScriptCoreLink::s_Instance = nullptr;

void ScriptCoreLink::Init(EngineAPI* eAPI, ComponentAPI* cAPI)
{
    s_Instance = std::make_unique<ScriptCoreLink>(eAPI, cAPI);
}

void ScriptCoreLink::ShutDown()
{
    getInstance()->m_ComponentAPI = nullptr;
    getInstance()->m_EngineAPI = nullptr;
    s_RegistrationFunctions.clear();
    s_Instance.reset();
}

ScriptCoreLink* ScriptCoreLink::getInstance()
{
    if (!s_Instance) {
        std::cout << "TRIED TO GET NULL SCRIPTE CORE LINK INSTANCE" << std::endl;
        return nullptr;
    }
    return s_Instance.get();
};

ScriptCoreLink::ScriptCoreLink(EngineAPI* eAPI, ComponentAPI* cAPI)
{
    m_EngineAPI = eAPI;
    m_ComponentAPI = cAPI;
}
}
