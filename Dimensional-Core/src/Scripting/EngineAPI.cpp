#include <Scripting/EngineAPI.hpp>
#include <core.hpp>

void Log(const char* message)
{
    DM_INFO("FROM SCRIPT: {}", message);
}

struct EngineAPI {
    std::function<void(const char*)> Log;
};

static EngineAPI eAPI = {
    &Log
};
extern "C" EngineAPI* getEngineAPI()
{
    return &eAPI;
}
