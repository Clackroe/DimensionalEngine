#ifndef DM_NATIVESCRIPTMANAGER
#define DM_NATIVESCRIPTMANAGER
#include "Scene/Scene.hpp"
#include <EngineAPI.hpp>
#include <core.hpp>
#include <cstring>

#ifdef _WIN32
#include <Windows.h>
#define LoadLibraryFunc(name) LoadLibrary(name)
#define GetFunctionAddress(lib, func) GetProcAddress((HMODULE)lib, func)
#define FreeLibraryFunc(lib) FreeLibrary((HMODULE)lib)
#define LibError() nullptr
#else
#include <dlfcn.h>
#define LoadLibraryFunc(name) dlopen(name, RTLD_LAZY | RTLD_LOCAL)
#define GetFunctionAddress(lib, func) dlsym(lib, func)
#define FreeLibraryFunc(lib) dlclose(lib)
#define LibError() dlerror()
#endif

struct ScriptInstance {

    NativeScriptableEntity* instancePTR;
};

#define MAX_MEMBERDATA_SIZE 16
struct ScriptComponentMemberData {

    template <typename T>
    T getData()
    {
        size_t size = sizeof(T);
        if (size > MAX_MEMBERDATA_SIZE) {
            DM_CORE_ERROR("Size Mismatch in Getting ComponentMemberData");
        }
        return *(T*)data;
    };

    template <typename T>
    void setData(T iData)
    {
        size_t size = sizeof(T);
        if (size > MAX_MEMBERDATA_SIZE) {
            DM_CORE_ERROR("Size Mismatch in Setting ComponentMemberData");
        }
        memcpy(data, &iData, size);
    }

    ScriptMemberType dataType = ScriptMemberType::NONE;

    u8 data[MAX_MEMBERDATA_SIZE];
};

// template <typename FuncT>
// bool loadLibraryFunction(void* libHandle, const char* funcName, std::function<FuncT>& out)
// {
//     if (!funcPTR) {
//         DM_CORE_WARN("FAILED TO LOAD FUNCTION {}", funcName);
//         return false;
//     }
//     auto typedFunc = reinterpret_cast<FuncT*>(funcPTR);
//     out = std::function<FuncT>(*typedFunc);
//     return true;
// }

namespace Dimensional {

class NativeScriptManager {
public:
    NativeScriptManager();
    ~NativeScriptManager();

    void reloadGameLibrary(const std::string& path);
    void freeGameLibrary();

private:
    void* m_GameLibraryHandle = nullptr;

    // TODO: Rework to use UUID (u64) for improved perfomance
    UMap<std::string, ScriptableEntityData> m_ReflectedClassData;

    UMap<UUID, std::vector<ScriptComponentMemberData>> m_ComponentMembers;

    NativeScriptRegistry* (*m_InitializeFunction)(EngineAPI*, ComponentAPI*) = nullptr;
    void (*m_CleanupFunction)(void) = nullptr;

    // TODO: May be redundant. ATM its essentially an unecessary abstraction of m_ReflectedClassData
    NativeScriptRegistry* m_NativeScriptRegistry;

    friend class Scene;
    friend class Application;
};
}

#endif
