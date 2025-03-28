#ifndef DM_ENGINEAPI
#define DM_ENGINEAPI
#include <KeyCodes.hpp>
#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <unordered_map>

enum class ScriptMemberType {
    FLOAT,
    INT,
    U32,
    U64,
    GLM_VEC3,
    NONE
};

namespace Dimensional {
 //static std::map<std::string, ScriptMemberType> g_StringToScriptMember;
 //static std::map<ScriptMemberType, std::string> g_ScriptMemberToString;
 //static std::map<ScriptMemberType, size_t> g_ScriptMemberToSize;


static std::map<std::string, ScriptMemberType> g_StringToScriptMember = {
    { "float", ScriptMemberType::FLOAT },
    { "int", ScriptMemberType::INT },
    { "uint32_t", ScriptMemberType::U32 },
    { "u32", ScriptMemberType::U32 },
    { "uint64_t", ScriptMemberType::U64 },
    { "u64", ScriptMemberType::U64 },
    { "glm::vec3", ScriptMemberType::GLM_VEC3 },
    { "vec3", ScriptMemberType::GLM_VEC3 }, // Just in case they use namespace glm
};

static std::map<ScriptMemberType, std::string> g_ScriptMemberToString = {
    { ScriptMemberType::FLOAT, "float" },
    { ScriptMemberType::INT, "int" },
    { ScriptMemberType::U32, "u32" },
    { ScriptMemberType::U64, "u64" },
    { ScriptMemberType::GLM_VEC3, "glm::vec3" },
};

static std::map<ScriptMemberType, size_t> g_ScriptMemberToSize = {
    { ScriptMemberType::FLOAT, sizeof(float) },
    { ScriptMemberType::INT, sizeof(int) },
    { ScriptMemberType::U32, sizeof(unsigned int) },
    { ScriptMemberType::U64, sizeof(unsigned long) },
    { ScriptMemberType::GLM_VEC3, sizeof(glm::vec3) },
};
}

namespace ScriptingCore {
class NativeScriptableEntity;
}

#define MAX_MEMBERDATA_SIZE 16 // Bytes
struct MemberData {
    std::string varName;
    char defaultVal[MAX_MEMBERDATA_SIZE];
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
