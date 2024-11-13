#include <Scripting/include/EngineAPI.hpp>
#include <core.hpp>

void LogInfo(const char* message)
{
    DM_INFO("FROM SCRIPT: {}", message);
}

void LogWarn(const char* message)
{
    DM_WARN("FROM SCRIPT: {}", message);
}
void LogError(const char* message)
{
    DM_ERROR("FROM SCRIPT: {}", message);
}

#define INIT_API_FUNC(name) eAPI.name = &name

EngineAPI g_EngineAPI = {
    .LogInfo = LogInfo,
    .LogWarn = LogWarn,
    .LogError = LogError

};

extern "C" EngineAPI* getEngineAPI()
{
    return &g_EngineAPI;
}
