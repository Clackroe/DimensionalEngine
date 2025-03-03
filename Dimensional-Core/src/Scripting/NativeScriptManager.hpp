#ifndef DM_NATIVESCRIPTMANAGER
#define DM_NATIVESCRIPTMANAGER
#include "Scene/Scene.hpp"
#include "Scene/SceneSerializer.hpp"
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

struct ScriptComponentMember {
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
        sizeBytes = size;

        dataChanged = true;
    }

    ScriptMemberType dataType = ScriptMemberType::NONE;
    std::string name = "NULL MEMBER";

    bool dataChanged = false;

    unsigned char data[MAX_MEMBERDATA_SIZE];
    int sizeBytes = 0;
};

struct ScriptInstanceMember {

    ScriptInstanceMember(MemberData* data)
    {
        m_MemberData = data;
    }

    ScriptMemberType getType() const { return m_MemberData->dataType; }
    std::string getName() const { return m_MemberData->varName; }

    template <typename T>
    T getData(ScriptingCore::NativeScriptableEntity* instance)
    {
        return *reinterpret_cast<T*>(m_MemberData->getter(instance));
    }

    template <typename T>
    void setData(ScriptingCore::NativeScriptableEntity* instance, T data)
    {
        m_MemberData->setter(instance, reinterpret_cast<void*>(&data));
    }

private:
    MemberData* m_MemberData;
    friend class NativeScriptManager;
};

struct ScriptInstance {

    ScriptInstance(ReflectedData classData, u64 entityHandle)
    {
        m_ClassData = classData;
        m_Instance = m_ClassData.classFactory(entityHandle);
        for (auto& member : m_ClassData.memberData) {
            members.insert({ member.varName, ScriptInstanceMember(&member) });
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

    UMap<std::string, ScriptInstanceMember> members;
    std::string className;

private:
    ScriptingCore::NativeScriptableEntity* m_Instance;
    ReflectedData m_ClassData;

    friend class NativeScriptManager;
};

struct ScriptComponantData {

    std::string className;
};

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
class NativeScriptManager {
public:
    NativeScriptManager();
    ~NativeScriptManager();

    void reloadGameLibrary(const std::string& path);
    void freeGameLibrary();

    void updateComponentMemberData();

    void onSceneStart();
    void onSceneUpdate();
    void onSceneEnd();

private:
    std::string replaceOldLibrary(const std::string& newPath);

private:
    void* m_GameLibraryHandle = nullptr;

    // TODO: Rework to use UUID (u64) for improved perfomance
    NativeScriptRegistry* m_NativeScriptRegistry;

    std::vector<ScriptInstance*> m_Instances;

    UMap<UUID, UMap<std::string, ScriptComponentMember>> m_ComponentMembers;

    friend class Scene;
    friend class Application;
    friend class SceneHierarchy;
    friend class EntitySerialzer;
};
}

#endif
