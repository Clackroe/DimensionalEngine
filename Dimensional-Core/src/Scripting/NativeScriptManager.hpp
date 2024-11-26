#ifndef DM_NATIVESCRIPTMANAGER
#define DM_NATIVESCRIPTMANAGER
#include "Scene/Scene.hpp"
#include <EngineAPI.hpp>
#include <core.hpp>
#include <cstring>
#include <vector>

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

namespace Dimensional {

struct ScriptMember {

    ScriptMember(MemberData* data)
    {
        m_MemberData = data;
    }

    ScriptMemberType getType() const { return m_MemberData->dataType; }
    std::string getName() const { return m_MemberData->varName; }

    template <typename T>
    T getData(NativeScriptableEntity* instance)
    {
        return *reinterpret_cast<T*>(m_MemberData->getter(instance));
    }

    template <typename T>
    void setData(NativeScriptableEntity* instance, T data)
    {
        m_MemberData->setter(instance, reinterpret_cast<void*>(&data));
    }

private:
    MemberData* m_MemberData;
};

struct ScriptInstance {

    ScriptInstance(ScriptableEntityData classData, u64 entityHandle)
    {
        m_ClassData = classData;
        m_Instance = m_ClassData.classFactory(entityHandle);
        for (auto& member : m_ClassData.memberData) {
            members.insert({ member.varName, ScriptMember(&member) });
        }
        onCreate();
    }

    ~ScriptInstance()
    {
        onDestroy();
        m_ClassData.classDestructor(m_Instance);
    }

    void onCreate()
    {
        m_ClassData.onCreate(m_Instance);
    };
    void onUpdate()
    {
        m_ClassData.onUpdate(m_Instance);
    };
    void onDestroy()
    {
        m_ClassData.onDestroy(m_Instance);
    };

    template <typename T>
    T getData(std::string& varName)
    {
        if (members.contains(varName)) {
            return members.at(varName).getData<T>(m_Instance);
        }
    }

    template <typename T>
    void setData(std::string& varName, T value)
    {
        if (members.contains(varName)) {
            return members.at(varName).setData<T>(m_Instance, value);
        }
    }

    UMap<std::string, ScriptMember> members;
    std::string className;

private:
    NativeScriptableEntity* m_Instance;
    ScriptableEntityData m_ClassData;
};

struct ScriptComponantData {

    std::string className;
};

template <typename FuncT>
bool loadLibraryFunction(void* libHandle, const char* funcName, std::function<FuncT>& out)
{
    void* funcPTR = dlsym(libHandle, funcName);
    if (!funcPTR) {
        DM_CORE_WARN("FAILED TO LOAD FUNCTION {}", funcName);
        return false;
    }
    auto typedFunc = reinterpret_cast<FuncT*>(funcPTR);
    out = std::function<FuncT>(*typedFunc);
    return true;
}

class NativeScriptManager {
public:
    NativeScriptManager();
    ~NativeScriptManager();

    void reloadGameLibrary(const std::string& path);
    void freeGameLibrary();

private:
    std::string replaceOldLibrary(const std::string& newPath);

private:
    void* m_GameLibraryHandle = nullptr;

    NativeScriptRegistry m_Registry;

    // TODO: Rework to use UUID (u64) for improved perfomance
    UMap<std::string, ScriptableEntityData> m_ReflectedClassData;

    std::vector<ScriptInstance*> m_Instances;

    // UMap<UUID, std::vector<ScriptComponentMemberData>> m_ComponentMembers;

    // TODO: May be redundant. ATM its essentially an unecessary abstraction of m_ReflectedClassData
    NativeScriptRegistry* m_NativeScriptRegistry;

    friend class Scene;
    friend class Application;
};
}

#endif
