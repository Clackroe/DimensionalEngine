#include <ReflectionHelpers.hpp>

#include <EngineAPI.hpp>

EngineAPI* g_EngineAPI = nullptr;
NativeScriptRegistry g_ScriptRegistry;
extern std::vector<std::function<void()>> g_RegistrationFunctions;

void registerScripts()
{
    for (auto& func : g_RegistrationFunctions) {
        func();
    }
}

DM_GAMEAPI NativeScriptRegistry* InitializeEngineAPI(EngineAPI* api)
{
    registerScripts();
    g_EngineAPI = api;
    return &g_ScriptRegistry;
}
