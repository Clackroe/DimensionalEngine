#include <EngineAPI.hpp>
#include <ReflectionHelpers.hpp>

EngineAPI* g_EngineAPI = nullptr;
ComponentAPI* g_CompAPI = nullptr;

NativeScriptRegistry g_ScriptRegistry;
extern std::vector<std::function<void()>> g_RegistrationFunctions;

void registerScripts()
{
    for (auto& func : g_RegistrationFunctions) {
        func();
    }
}

DM_GAMEAPI NativeScriptRegistry* InitializeEngineAPI(EngineAPI* engineAPI, ComponentAPI* componentAPI)
{
    registerScripts();
    g_EngineAPI = engineAPI;
    g_CompAPI = componentAPI;
    return &g_ScriptRegistry;
}

DM_GAMEAPI void cleanupAPI()
{
    std::cout << "Cleaning Up GameLibrary" << std::endl;
    g_EngineAPI = nullptr;
    g_CompAPI = nullptr;
    g_ScriptRegistry = NativeScriptRegistry();
    g_RegistrationFunctions.clear();
}
