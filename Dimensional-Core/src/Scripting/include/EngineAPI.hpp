#ifndef DM_ENGINEAPI
#define DM_ENGINEAPI
#include <functional>
#include <map>
#include <string>
#include <unordered_map>

class NativeScriptableEntity;

// Each Dimensional Script will require a constructor that takes in an entity handle
struct ScriptableEntityData {
    // Class and Instance Data
    std::string className;
    std::function<NativeScriptableEntity*(int)> classFactory = nullptr;
    std::function<void()> classDestructor = nullptr;
    // Events
    std::function<void()> onUpdate = nullptr;
    std::function<void()> onCreate = nullptr;
    std::function<void()> onDestroy = nullptr;
};

struct NativeScriptRegistry {
    std::unordered_map<std::string, ScriptableEntityData> scriptRegistry;
};

#define REGISTER_API_FUNC(name) std::function<decltype(name)> name;

struct EngineAPI {
    std::function<void(const char*)> LogInfo;
    std::function<void(const char*)> LogWarn;
    std::function<void(const char*)> LogError;
};

extern "C" EngineAPI* getEngineAPI();

#endif
