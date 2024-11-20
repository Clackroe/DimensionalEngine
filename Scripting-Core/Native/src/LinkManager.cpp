#include <EngineAPI.hpp>
#include <NativeScripting.hpp>
#include <ReflectionHelpers.hpp>

void registerScripts()
{
    for (auto& func : ScriptCoreLink::s_RegistrationFunctions) {
        func();
    }
}

DM_GAMEAPI NativeScriptRegistry* InitializeEngineAPI(EngineAPI* engineAPI, ComponentAPI* componentAPI)
{
    ScriptCoreLink::Init(engineAPI, componentAPI);
    registerScripts();
    return &ScriptCoreLink::s_ScriptRegistry;
}

DM_GAMEAPI void cleanupAPI()
{
    std::cout << "Cleaning Up GameLibrary" << std::endl;
    ScriptCoreLink::ShutDown();
}
