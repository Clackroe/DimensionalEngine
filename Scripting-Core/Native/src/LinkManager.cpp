#include <CoreScriptLib.hpp>
#include <EngineAPI.hpp>

EngineAPI* engineAPI = nullptr;

ScriptableEntityData t {};

void UpdateTest()
{
    engineAPI->LogWarn("WOW ITS A WARNING");
}

DM_GAMEAPI ScriptableEntityData* Init(EngineAPI* api)
{
    std::cout << "INITIALIZING SCRIPTING" << std::endl;
    t.onUpdate = UpdateTest;
    engineAPI = api;

    return &t;
}
