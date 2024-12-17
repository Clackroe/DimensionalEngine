#include "Input/Input.hpp"
#include <EngineAPI.hpp>
#include <core.hpp>

namespace Dimensional {
std::map<std::string, ScriptMemberType> g_StringToScriptMember = {
    { "float", ScriptMemberType::FLOAT },
    { "int", ScriptMemberType::INT },
    { "uint32_t", ScriptMemberType::U32 },
    { "u32", ScriptMemberType::U32 },
    { "uint64_t", ScriptMemberType::U64 },
    { "u64", ScriptMemberType::U64 },
    { "glm::vec3", ScriptMemberType::GLM_VEC3 },
    { "vec3", ScriptMemberType::GLM_VEC3 }, // Just in case they use namespace glm
};

namespace DimensionalScriptAPI {

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

    // === INPUT ===
    bool IsKeyUp(Dimensional::KeyCode key)
    {
        return Dimensional::Input::isKeyReleased(key);
    }
    bool IsKeyDown(Dimensional::KeyCode key)
    {
        return Dimensional::Input::isKeyDown(key);
    }
    bool IsMouseDown(Dimensional::MouseCode key)
    {
        return Dimensional::Input::isMouseDown(key);
    }

    bool IsMouseUp(Dimensional::MouseCode key)
    {
        return Dimensional::Input::isMouseReleased(key);
    }

    float GetMouseX()
    {
        return Dimensional::Input::getMouseX();
    }

    float GetMouseY()
    {
        return Dimensional::Input::getMouseY();
    }

    EngineAPI g_EngineAPI = {
        .LogInfo = LogInfo,
        .LogWarn = LogWarn,
        .LogError = LogError,
        .Input_IsKeyDown = IsKeyDown,
        .Input_IsKeyUp = IsKeyUp,
        .Input_IsMouseDown = IsMouseUp,
        .Input_IsMouseUp = IsMouseDown,
        .Input_GetMouseX = GetMouseX,
        .Input_GetMouseY = GetMouseY,

    };

    extern "C" EngineAPI* getEngineAPI()
    {
        return &g_EngineAPI;
    }
}
}
