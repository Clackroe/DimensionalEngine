#ifndef DM_NATIVESCRIPTMANAGER
#define DM_NATIVESCRIPTMANAGER
#include "Scripting/include/EngineAPI.hpp"
#include <functional>
#ifdef _WIN32
#include <Windows.h>
#define LoadLibraryFunc(name) LoadLibrary(name)
#define GetFunctionAddress(lib, func) GetProcAddress((HMODULE)lib, func)
#define FreeLibraryFunc(lib) FreeLibrary((HMODULE)lib)
#define LibError() nullptr
#else
#include <dlfcn.h>
#define LoadLibraryFunc(name) dlopen(name, RTLD_NOW | RTLD_LOCAL)
#define GetFunctionAddress(lib, func) dlsym(lib, func)
#define FreeLibraryFunc(lib) dlclose(lib)
#define LibError() dlerror()
#endif

template <typename FuncT>
bool loadLibraryFunction(void* libHandle, const char* funcName, std::function<FuncT>& out)
{
    void* funcPTR = GetFunctionAddress(libHandle, funcName);
    if (!funcPTR) {
        DM_CORE_WARN("FAILED TO LOAD FUNCTION {}", funcName);
        return false;
    }
    auto typedFunc = reinterpret_cast<FuncT*>(funcPTR);
    out = std::function<FuncT>(*typedFunc);
    return true;
}

#include <core.hpp>

namespace Dimensional {

class NativeScriptManager {
public:
    NativeScriptManager();
    ~NativeScriptManager();

    void reloadGameLibrary(const std::string& path);
    void freeGameLibrary();

private:
    void* m_GameLibraryHandle = nullptr;

    std::function<NativeScriptRegistry*(EngineAPI*)> m_InitializeFunction = nullptr;
    NativeScriptRegistry* m_NativeScriptRegistry;

    friend class Scene;
    friend class Application;
};
}

#endif
