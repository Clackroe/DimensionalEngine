#include "Core/Application.hpp"
#include "EngineAPI.hpp"
#include "Log/log.hpp"
#include "Scene/Components.hpp"
#include <Scripting/NativeScriptManager.hpp>
#include <dlfcn.h>
#include <filesystem>

extern "C" EngineAPI* getEngineAPI();
extern "C" ComponentAPI* getComponentAPI();

namespace Dimensional {

NativeScriptManager::NativeScriptManager()
{
}
NativeScriptManager::~NativeScriptManager()
{
}

void NativeScriptManager::reloadGameLibrary(const std::string& path)
{

    freeGameLibrary();

    std::string uniqueLibPath = "Assets/.bin/Game/libGameApp_" + std::to_string(std::time(nullptr)) + ".so";

    if (std::filesystem::exists("Assets/.bin/Game")) {
        std::filesystem::remove_all("Assets/.bin/Game");
    }
    std::filesystem::create_directories("Assets/.bin/Game");
    std::filesystem::copy_file(path, uniqueLibPath);

    m_GameLibraryHandle = dlopen(uniqueLibPath.c_str(), RTLD_NOW);
    if (!m_GameLibraryHandle) {
        const char* error = dlerror();
        if (error) {
            DM_CORE_WARN("FAILED TO LOAD GAME LIBRARY: {}", error);
        } else {
            DM_CORE_WARN("FAILED TO LOAD GAME LIBRARY: Unknown error");
        }
        return;
    }

    m_InitializeFunction = reinterpret_cast<NativeScriptRegistry* (*)(EngineAPI*, ComponentAPI*)>(
        dlsym(m_GameLibraryHandle, "InitializeEngineAPI"));

    // bool success = loadLibraryFunction(m_GameLibraryHandle, "InitializeEngineAPI", m_InitializeFunction);
    if (!m_InitializeFunction) {
        DM_CORE_WARN("Cannot retrieve initializeFunction from game library");
        m_GameLibraryHandle = nullptr;
        m_InitializeFunction = nullptr;
        return;
    }

    m_CleanupFunction = reinterpret_cast<void (*)()>(
        dlsym(m_GameLibraryHandle, "cleanupAPI"));

    // m_CleanupFunction = loadLibraryFunction(m_GameLibraryHandle, "cleanupAPI", m_CleanupFunction);
    if (!m_CleanupFunction) {
        DM_CORE_WARN("Cannot retrieve  cleanupFunction from game library");
        m_GameLibraryHandle = nullptr;
        m_CleanupFunction = nullptr;
        return;
    }

    EngineAPI* api = getEngineAPI();
    ComponentAPI* cAPI = getComponentAPI();
    NativeScriptRegistry* tempReg = m_InitializeFunction(api, cAPI);
    if (tempReg) {
        m_NativeScriptRegistry = tempReg;
        tempReg = nullptr;
    } else {
        DM_CORE_WARN("Failed to retrieve reflection data for Native Script");
        return;
    }

    // Fill Component Members
    // I still dont really love this. I may replace this as the system evolves.
    // m_ReflectedClassData = m_NativeScriptRegistry->scriptRegistry;

    auto scene = Application::getApp().getSceneCTX();
    if (scene) {
        auto view = scene->getAllEntitiesWith<IDComponent, NativeScriptComponent>();
        for (auto e : view) {
            auto [idComp, comp] = view.get<IDComponent, NativeScriptComponent>(e);
            ScriptableEntityData& classData = m_NativeScriptRegistry->at(comp.className);

            DM_CORE_WARN("Class: {}", classData.className);
            // std::vector<ScriptComponentMemberData> compMemberList = m_ComponentMembers[idComp.ID];

            for (auto& member : classData.memberData) {
                ScriptComponentMemberData data {};
                data.dataType = member.dataType;
                // data.setData<float>(*(float*)member.defaultVal);
                // float temp = data.getData<float>();
                std::string t = member.varName;
                // DM_CORE_WARN("Default: {0}:{1}", t, temp);
                // compMemberList.push_back(data);
            }
        }
    }
}

void NativeScriptManager::freeGameLibrary()
{
    m_ReflectedClassData.clear();
    for (auto& [k, v] : m_ComponentMembers) {
        v.clear();
    }
    m_ComponentMembers.clear();
    if (m_NativeScriptRegistry) {
        for (auto& [k, v] : *m_NativeScriptRegistry) {
            v.onCreate = nullptr;
            v.onUpdate = nullptr;
            v.onDestroy = nullptr;
            v.classFactory = nullptr;
            v.classDestructor = nullptr;
            for (auto& m : v.memberData) {
                m.getter = nullptr;
                m.setter = nullptr;
            }
        }
        m_NativeScriptRegistry->clear();
    }

    if (m_GameLibraryHandle) {

        if (m_CleanupFunction) {
            m_CleanupFunction();
            m_CleanupFunction = nullptr;
        }

        m_NativeScriptRegistry = nullptr;
        m_InitializeFunction = nullptr;

        if (dlclose(m_GameLibraryHandle) != 0) {
            const char* error = dlerror();
            DM_CORE_WARN("FAILED TO UNLOAD GAME LIBRARY: {}", error);
        }

        m_GameLibraryHandle = nullptr;
    }
}

}
