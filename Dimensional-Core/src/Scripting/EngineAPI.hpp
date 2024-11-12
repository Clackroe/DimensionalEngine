#ifndef DM_ENGINEAPI
#define DM_ENGINEAPI
#include <functional>
#include <map>
#include <string>

// Each Dimensional Script will require a constructor that takes in an entity handle
struct ScriptableEntity {
    std::string className;
    std::function<void(uint32_t)> classFactory = nullptr;
    std::function<void()> onUpdate = nullptr;
    std::function<void()> onCreate = nullptr;
    std::function<void()> onDestroy = nullptr;
};

struct ScriptRegistry {
    std::map<uint64_t, ScriptableEntity> registry;
};

struct EngineAPI; // Forward Declare

extern "C" EngineAPI* getEngineAPI();

#endif
