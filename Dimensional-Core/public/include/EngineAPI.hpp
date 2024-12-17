#ifndef DM_ENGINEAPI
#define DM_ENGINEAPI
#include <KeyCodes.hpp>
#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <unordered_map>

class NativeScriptableEntity;

enum class ScriptMemberType {
    FLOAT,
    INT,
    U32,
    U64,
    GLM_VEC3,
    NONE
};

namespace Dimensional {
extern std::map<std::string, ScriptMemberType> g_StringToScriptMember;
}

struct MemberData {
    std::string varName;
    size_t offsetBytes;
    ScriptMemberType dataType = ScriptMemberType::NONE;
    std::function<void*(NativeScriptableEntity*)> getter;
    std::function<void(NativeScriptableEntity*, void*)> setter;
};

// Each Dimensional Script will require a constructor that takes in an entity handle
struct ReflectedData {
    // Class and Instance Data
    std::string className;

    std::function<NativeScriptableEntity*(uint64_t)> classFactory = nullptr;
    std::function<void(NativeScriptableEntity*)> classDestructor = nullptr;

    // Events
    std::function<void(NativeScriptableEntity*)> onUpdate = nullptr;
    std::function<void(NativeScriptableEntity*)> onCreate = nullptr;
    std::function<void(NativeScriptableEntity*)> onDestroy = nullptr;

    std::vector<MemberData> memberData;
};

using NativeScriptRegistry = std::unordered_map<std::string, ReflectedData>;

// OPAQUE POINTERS
struct TransformCompHandle;

struct ComponentAPI {
    // TRANSFORM COMPONENT
    void* _dummy;
    std::function<TransformCompHandle*(uint64_t)> Transform_GetComp;
    std::function<glm::vec3(TransformCompHandle*)> Transform_GetPosition;
    std::function<void(TransformCompHandle*, glm::vec3)> Transform_SetPosition;

    // Camera
    // DirectionalLight
};

struct EngineAPI {
    // LOGGING
    void* _dummy;
    std::function<void(const char*)> LogInfo;
    std::function<void(const char*)> LogWarn;
    std::function<void(const char*)> LogError;
    // INPUT
    std::function<bool(Dimensional::KeyCode)> Input_IsKeyDown;
    std::function<bool(Dimensional::KeyCode)> Input_IsKeyUp;

    std::function<bool(Dimensional::MouseCode)> Input_IsMouseDown;
    std::function<bool(Dimensional::MouseCode)> Input_IsMouseUp;

    std::function<float()> Input_GetMouseX;
    std::function<float()> Input_GetMouseY;

    // ENTITY
};

#endif
