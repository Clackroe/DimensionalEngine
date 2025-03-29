#ifndef DM_ENGINEAPI
#define DM_ENGINEAPI
#include "glm/fwd.hpp"
#include <KeyCodes.hpp>
#include <cstdint>
#include <functional>
#include <glm/glm.hpp>

#include <ReflectionData.hpp>

namespace ScriptingCore {
class NativeScriptableEntity;
}

#define MAX_MEMBERDATA_SIZE 16 // Bytes
union MemberDataValue {
    char bytes[MAX_MEMBERDATA_SIZE];
    int intValue;
    float floatValue;
    uint32_t u32Value;
    uint64_t u64Value;
    glm::vec3 vec3Value;
};

struct MemberData {
    std::string varName;
    MemberDataValue defaultVal;
    ScriptMemberType dataType = ScriptMemberType::NONE;
    std::function<void*(ScriptingCore::NativeScriptableEntity*)> getter;
    std::function<void(ScriptingCore::NativeScriptableEntity*, void*)> setter;
};

// Each Dimensional Script will require a constructor that takes in an entity handle
struct ReflectedData {
    // Class and Instance Data
    std::string className;

    std::function<ScriptingCore::NativeScriptableEntity*(uint64_t)> classFactory = nullptr;
    std::function<void(ScriptingCore::NativeScriptableEntity*)> classDestructor = nullptr;

    // Events
    std::function<void(ScriptingCore::NativeScriptableEntity*)> onUpdate = nullptr;
    std::function<void(ScriptingCore::NativeScriptableEntity*)> onCreate = nullptr;
    std::function<void(ScriptingCore::NativeScriptableEntity*)> onDestroy = nullptr;

    std::vector<MemberData> memberData;
};

using NativeScriptRegistry = std::unordered_map<std::string, ReflectedData>;

// OPAQUE POINTERS
struct ComponentHandle;

using TransformCompHandle = ComponentHandle;

struct ComponentAPI {
    // TRANSFORM COMPONENT
    void* _dummy;
    std::function<TransformCompHandle*(uint64_t)> Transform_GetComp;

    std::function<glm::vec3(TransformCompHandle*)> Transform_GetPosition;
    std::function<void(TransformCompHandle*, glm::vec3)> Transform_SetPosition;

    std::function<glm::vec3(TransformCompHandle*)> Transform_GetRotationRadians;
    std::function<glm::vec3(TransformCompHandle*)> Transform_GetRotationDegrees;
    std::function<void(TransformCompHandle*, glm::vec3)> Transform_SetRotationRadians;
    std::function<void(TransformCompHandle*, glm::vec3)> Transform_SetRotationDegrees;

    std::function<glm::vec3(TransformCompHandle*)> Transform_GetScale;
    std::function<void(TransformCompHandle*, glm::vec3)> Transform_SetScale;

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
