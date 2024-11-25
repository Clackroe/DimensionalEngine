#include <DefinesScriptLib.hpp>
#include <EngineAPI.hpp>
#include <NativeScripting.hpp>
#include <ReflectionHelpers.hpp>
#include <iostream>

namespace ScriptingCore {

static void registerScripts(NativeScriptRegistry* reg)
{

    for (auto& func : ScriptCoreLink::s_RegistrationFunctions) {
        std::cout << "Loading Script..." << std::endl;
        func(reg);
    }
}

DM_GAMEAPI void Initialize(EngineAPI* eAPI, ComponentAPI* compAPI, NativeScriptRegistry* registry)
{
    std::cout << "Test: FROM LIBRARY ------ ERM7 BO000O" << std::endl;
    ScriptCoreLink::Init(eAPI, compAPI);
    registerScripts(registry);
}

DM_GAMEAPI void Cleanup()
{
    std::cout << "Cleaning Up Library" << std::endl;
    ScriptCoreLink::ShutDown();
}
}
