#include "Core/Application.hpp"
#include "EngineAPI.hpp"
#include "Log/log.hpp"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include <Scripting/NativeScriptManager.hpp>
#include <filesystem>
#include <sys/socket.h>

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
    const char* error = LibError();
    if (error) {
        DM_CORE_WARN("{0}: {1}", message, error);
    } else {
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

            // inputed clas doesnt exist in game library
            if (!m_NativeScriptRegistry->contains(SComp.className)) {
                continue;
            }

            // Meta data for inputted class
            ReflectedData& classData = m_NativeScriptRegistry->at(SComp.className);

            // Update members in script component to abide by reflected data
            UMap<std::string, ScriptComponentMember> tempMembers;
            for (MemberData& data : classData.memberData) {

                ScriptComponentMember m = {
                    .dataType = data.dataType,
                    .name = data.varName,
                    .sizeBytes = (int)g_ScriptMemberToSize[data.dataType],
                };

                memcpy(m.data, &data.defaultVal, MAX_MEMBERDATA_SIZE);

                tempMembers.insert({ m.name, m });
            }

            // Insert default member data
            temp.insert({ IDComp.ID, tempMembers });
        }

        // Update the member data if the user has changed it in the editor
        for (auto& [id, members] : temp) {

            if (!m_ComponentMembers.contains(id)) {
                continue;
            }

            // Loop through members
            for (auto& [name, mData] : members) {
                // Could be a new member or data type could have changed
                if (!m_ComponentMembers.at(id).contains(name)) {
                    continue;
                }
                ScriptComponentMember& oldMember = m_ComponentMembers.at(id).at(name);
                ScriptComponentMember& newMember = temp.at(id).at(name);

                if (oldMember.dataType != newMember.dataType) {
                    continue;
                }

                if (oldMember.dataChanged) {
                    newMember = oldMember;
                }
            }
        }

        m_ComponentMembers = temp;
    } else {
        DM_CORE_WARN("No Scene Context")
    }
};

void NativeScriptManager::reloadGameLibrary(const std::string& path)
{

    freeGameLibrary();
    std::string newPath = replaceOldLibrary(path);

    m_GameLibraryHandle = LoadLibraryFunc(newPath.c_str());

    if (!m_GameLibraryHandle) {
        printLibError("FAILED TO LOAD GAME LIBRARY");
    }

    std::function<void(EngineAPI*, ComponentAPI*, NativeScriptRegistry*)> initFunc;
    loadLibraryFunction(m_GameLibraryHandle, "Initialize", initFunc);

    if (!initFunc) {
        printLibError("Cannot retrieve initializeFunction from game library");
        if (m_GameLibraryHandle) {
            FreeLibraryFunc(m_GameLibraryHandle);
        }
        m_GameLibraryHandle = nullptr;
        return;
    }

    EngineAPI* api = DimensionalScriptAPI::getEngineAPI();
    ComponentAPI* cAPI = ComponantScriptAPI::getComponentAPI();

    NativeScriptRegistry* oldReg = m_NativeScriptRegistry;

    m_NativeScriptRegistry = new NativeScriptRegistry();
    initFunc(api, cAPI, m_NativeScriptRegistry);

    updateComponentMemberData();
}

void NativeScriptManager::onSceneStart()
{
    Ref<Scene> sc = Application::getApp().getSceneCTX();
    auto v = sc->getAllEntitiesWith<IDComponent, NativeScriptComponent>();
    for (auto& e : v) {
        auto [IDComp, SComp] = v.get<IDComponent, NativeScriptComponent>(e);

        if (m_NativeScriptRegistry->contains(SComp.className)) {
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
            memset(m.defaultVal, 0, MAX_MEMBERDATA_SIZE);
        }
        v.memberData.clear();
    }
    m_NativeScriptRegistry->clear();

    for (auto& v : m_Instances) {
        v->m_Instance = nullptr;

        v->m_ClassData.onCreate = nullptr;
        v->m_ClassData.onUpdate = nullptr;
        v->m_ClassData.onDestroy = nullptr;
        v->m_ClassData.className = "";
        v->m_ClassData.classFactory = nullptr;
        v->m_ClassData.classDestructor = nullptr;
        for (auto& m : v->m_ClassData.memberData) {
            m.varName = "";
            m.getter = nullptr;
            m.setter = nullptr;
            m.dataType = ScriptMemberType::NONE;
        }
        v->m_ClassData.memberData.clear();
        v->members.clear();
    }

    m_Instances.clear();

    std::function<void()> cleanupFunction;
    loadLibraryFunction(m_GameLibraryHandle, "Cleanup", cleanupFunction);

    if (m_NativeScriptRegistry) {
        delete m_NativeScriptRegistry;
    }

    if (!cleanupFunction) {
        printLibError("Cannot retrieve cleanup from game library");
        if (m_GameLibraryHandle) {
            FreeLibraryFunc(m_GameLibraryHandle);
        }
        m_GameLibraryHandle = nullptr;
        return;
    }

    cleanupFunction = nullptr;

    if (m_GameLibraryHandle) {
        FreeLibraryFunc(m_GameLibraryHandle);
        printLibError("Cannot unload game library");
    }
}
}
