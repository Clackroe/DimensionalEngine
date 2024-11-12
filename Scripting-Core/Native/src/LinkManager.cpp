#include <CoreScriptLib.hpp>
#include <EngineAPI.hpp>

EngineAPI* engineAPI = nullptr;
DM_GAMEAPI void InitializeScriptEngine(EngineAPI* api)
{
    engineAPI = api;
}
