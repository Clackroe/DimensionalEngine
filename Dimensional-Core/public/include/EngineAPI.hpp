#ifndef DM_ENGINEAPI
#define DM_ENGINEAPI
#include <KeyCodes.hpp>
#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class NativeScriptableEntity;

// Each Dimensional Script will require a constructor that takes in an entity handle
struct ScriptableEntityData {
    // Class and Instance Data
    std::string className;
    std::function<NativeScriptableEntity*(uint64_t)> classFactory = nullptr;
    std::function<void(NativeScriptableEntity*)> classDestructor = nullptr;
    // Events
    std::function<void(NativeScriptableEntity*)> onUpdate = nullptr;
    std::function<void(NativeScriptableEntity*)> onCreate = nullptr;
    std::function<void(NativeScriptableEntity*)> onDestroy = nullptr;
};

struct NativeScriptRegistry {
    std::unordered_map<std::string, ScriptableEntityData> scriptRegistry = std::unordered_map<std::string, ScriptableEntityData>();
};

// OPAQUE POINTERS
struct TransformCompHandle;

struct ComponentAPI {
    // TRANSFORM COMPONENT
    void* _dummy;
    std::function<TransformCompHandle*(uint64_t)> Transform_GetComp;
    std::function<glm::vec3(TransformCompHandle*)> Transform_GetPosition;
    std::function<void(TransformCompHandle*, glm::vec3)> Transform_SetPosition;
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

extern "C" EngineAPI* getEngineAPI();
extern "C" ComponentAPI* getComponentAPI();

#endif
