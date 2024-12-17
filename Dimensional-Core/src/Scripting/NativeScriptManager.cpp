#include "Core/Application.hpp"
#include "EngineAPI.hpp"
#include "Log/log.hpp"
#include "Scene/Components.hpp"
#include <Scripting/NativeScriptManager.hpp>
#include <cstring>
#include <dlfcn.h>
#include <filesystem>

namespace Dimensional {
namespace DimensionalScriptAPI {
    extern "C" EngineAPI* getEngineAPI();
}

namespace ComponantScriptAPI {
    extern "C" ComponentAPI* getComponentAPI();
}

NativeScriptManager::NativeScriptManager()
{
}
NativeScriptManager::~NativeScriptManager()
{
}

std::string NativeScriptManager::replaceOldLibrary(const std::string& newPath)
{
    std::string uniqueLibPath = "Assets/.bin/Game/libGameApp_" + std::to_string(std::time(nullptr)) + ".so";

    if (std::filesystem::exists("Assets/.bin/Game")) {
        std::filesystem::remove_all("Assets/.bin/Game");
    }
    std::filesystem::create_directories("Assets/.bin/Game");
    std::filesystem::copy_file(newPath, uniqueLibPath);
    return uniqueLibPath;
}

static void printLibError(const char* message)
{
    const char* error = dlerror();
    if (error) {
        DM_CORE_WARN("{0}: {1}", message, error);
    } else {
        DM_CORE_WARN("{0}: Unknown Error", message);
    }
    return;
};

void NativeScriptManager::updateComponentMemberData()
{

    Ref<Scene> sc = Application::getApp().getSceneCTX();
    if (m_NativeScriptRegistry && sc) {
        auto entities = sc->getAllEntitiesWith<IDComponent, NativeScriptComponent>();

        // Generate New Data | Just in case the user changed the members of scripts
        UMap<UUID, UMap<std::string, ScriptComponentMember>> temp;
        for (auto& e : entities) {
            auto [IDComp, SComp] = entities.get<IDComponent, NativeScriptComponent>(e);
            if (!m_NativeScriptRegistry->contains(SComp.className)) {
                continue;
            }
            ReflectedData& classData = m_NativeScriptRegistry->at(SComp.className);

            UMap<std::string, ScriptComponentMember> tempMembers;
            for (MemberData& data : classData.memberData) {

                ScriptComponentMember m = {
                    .dataType = data.dataType,
                    .name = data.varName
                };
                tempMembers.insert({ m.name, m });
            }

            temp.insert({ IDComp.ID, tempMembers });
        }

        // Update values with edior set counterparts, if applicable.
        for (auto& [id, scMembers] : temp) {
            if (!m_ComponentMembers.contains(id))
                continue;
            for (auto& [name, scMem] : scMembers) {
                if (!m_ComponentMembers.at(id).contains(name))
                    continue;

                auto& old = m_ComponentMembers.at(id).at(name);
                if (scMem.dataType != old.dataType) {
                    continue;
                }
                DM_CORE_WARN("UPDATING: {}", scMem.sizeBytes);
                std::memcpy(&scMem.data, &old.data, scMem.sizeBytes);
            }
        }

        m_ComponentMembers = temp;
    } else {
        DM_CORE_WARN("No Scene Context");
    }
}

void NativeScriptManager::reloadGameLibrary(const std::string& path)
{

    freeGameLibrary();
    std::string newPath = replaceOldLibrary(path);

    m_GameLibraryHandle = dlopen(newPath.c_str(), RTLD_NOW | RTLD_DEEPBIND);

    if (!m_GameLibraryHandle) {
        printLibError("FAILED TO LOAD GAME LIBRARY");
    }

    std::function<void(EngineAPI*, ComponentAPI*, NativeScriptRegistry*)> initFunc;
    loadLibraryFunction(m_GameLibraryHandle, "Initialize", initFunc);

    if (!initFunc) {
        printLibError("Cannot retrieve initializeFunction from game library");
        if (m_GameLibraryHandle) {
            dlclose(m_GameLibraryHandle);
        }
        m_GameLibraryHandle = nullptr;
        return;
    }

    EngineAPI* api = DimensionalScriptAPI::getEngineAPI();
    ComponentAPI* cAPI = ComponantScriptAPI::getComponentAPI();
    m_NativeScriptRegistry = new NativeScriptRegistry();
    initFunc(api, cAPI, m_NativeScriptRegistry);

    updateComponentMemberData();
    for (auto& [id, data] : m_ComponentMembers) {
        for (auto& [name, member] : data) {
            DM_CORE_INFO("{}", member.name);
        }
    }
}

void NativeScriptManager::onSceneStart()
{
    DM_CORE_WARN("SM Start")
    Ref<Scene> sc = Application::getApp().getSceneCTX();
    auto v = sc->getAllEntitiesWith<IDComponent, NativeScriptComponent>();
    DM_CORE_INFO("Size: {}", v.size_hint())
    for (auto& e : v) {
        auto [IDComp, SComp] = v.get<IDComponent, NativeScriptComponent>(e);

        if (m_NativeScriptRegistry->contains(SComp.className)) {
            DM_CORE_WARN("Creating : {}", SComp.className)

            auto data = m_NativeScriptRegistry->at(SComp.className);
            ScriptInstance* instance = new ScriptInstance(data, IDComp.ID);
            m_Instances.push_back(instance);
            for (auto& [name, member] : m_ComponentMembers.at(IDComp.ID)) {
                float data = m_ComponentMembers.at(IDComp.ID).at(name).getData<float>();
                instance->setData(*(std::string*)(&name), data);
            }
        }
    }
}

void NativeScriptManager::onSceneUpdate()
{
    for (auto& instance : m_Instances) {
        instance->onUpdate();
    }
}

void NativeScriptManager::onSceneEnd()
{
    for (auto& t : m_Instances) {
        delete t;
    }
    m_Instances.clear();
}

void NativeScriptManager::freeGameLibrary()
{
    if (!m_GameLibraryHandle) {
        return;
    }
    for (auto& [k, v] : *m_NativeScriptRegistry) {
        v.onCreate = nullptr;
        v.onUpdate = nullptr;
        v.onDestroy = nullptr;
        v.className = "";
        v.classFactory = nullptr;
        v.classDestructor = nullptr;
        for (auto& m : v.memberData) {
            m.varName = "";
            m.getter = nullptr;
            m.setter = nullptr;
            m.dataType = ScriptMemberType::NONE;
            m.offsetBytes = 0;
        }
        v.memberData.clear();
    }
    m_NativeScriptRegistry->clear();

    std::function<void()> cleanupFunction;
    loadLibraryFunction(m_GameLibraryHandle, "Cleanup", cleanupFunction);

    if (m_NativeScriptRegistry) {
        delete m_NativeScriptRegistry;
    }

    if (!cleanupFunction) {
        printLibError("Cannot retrieve cleanup from game library");
        if (m_GameLibraryHandle) {
            dlclose(m_GameLibraryHandle);
        }
        m_GameLibraryHandle = nullptr;
        return;
    }

    cleanupFunction = nullptr;

    if (m_GameLibraryHandle) {
        dlclose(m_GameLibraryHandle);
        printLibError("Cannot unload game library");
    }
}
}
