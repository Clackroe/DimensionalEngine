#include "Core/Application.hpp"
#include "EngineAPI.hpp"
#include "Log/log.hpp"
#include "Scene/Components.hpp"
#include <Scripting/NativeScriptManager.hpp>
#include <filesystem>
namespace Dimensional {

NativeScriptManager::NativeScriptManager()
{
}
NativeScriptManager::~NativeScriptManager()
{
}

void NativeScriptManager::reloadGameLibrary(const std::string& path)
{

    std::string uniqueLibPath = "Assets/.bin/Game/libGameApp_" + std::to_string(std::time(nullptr)) + ".so";

    if (std::filesystem::exists("Assets/.bin/Game")) {
        std::filesystem::remove_all("Assets/.bin/Game");
    }
    std::filesystem::create_directories("Assets/.bin/Game");
    std::filesystem::copy_file(path, uniqueLibPath);

    freeGameLibrary();
    m_GameLibraryHandle = LoadLibraryFunc(uniqueLibPath.c_str());
    if (!m_GameLibraryHandle) {
        const char* error = LibError();
        if (error) {
            DM_CORE_WARN("FAILED TO LOAD GAME LIBRARY: {}", error);
        } else {
            DM_CORE_WARN("FAILED TO LOAD GAME LIBRARY: Unknown error");
        }
        return;
    }

    bool success = loadLibraryFunction(m_GameLibraryHandle, "InitializeEngineAPI", m_InitializeFunction);
    if (!success) {
        DM_CORE_WARN("Cannot retrieve initializeFunction from game library");
        m_GameLibraryHandle = nullptr;
        m_InitializeFunction = nullptr;
        return;
    }

    success = loadLibraryFunction(m_GameLibraryHandle, "cleanupAPI", m_CleanupFunction);
    if (!success) {
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
        DM_CORE_WARN("Trying to initialize ComponentData")
        auto view = scene->getAllEntitiesWith<IDComponent, NativeScriptComponent>();
        for (auto e : view) {
            auto [idComp, comp] = view.get<IDComponent, NativeScriptComponent>(e);
            ScriptableEntityData& classData = m_NativeScriptRegistry->scriptRegistry.at(comp.className);

            DM_CORE_WARN("Class: {}", classData.className);
            // std::vector<ScriptComponentMemberData> compMemberList = m_ComponentMembers[idComp.ID];

            for (auto& member : classData.memberData) {
                ScriptComponentMemberData data {};
                data.dataType = member.dataType;
                data.setData<float>(*(float*)member.defaultVal);
                float temp = data.getData<float>();
                DM_CORE_WARN("Default: {0}:{1}", member.varName, temp);
                // compMemberList.push_back(data);
            }
        }
    }
}

void NativeScriptManager::freeGameLibrary()
{
    m_ReflectedClassData.clear();
    for (auto [k, v] : m_ComponentMembers) {
        v.clear();
    }
    m_ComponentMembers.clear();

    if (m_CleanupFunction) {
        m_CleanupFunction();
    }
    m_CleanupFunction = nullptr;

    if (m_GameLibraryHandle) {

        if (m_NativeScriptRegistry) {
            m_NativeScriptRegistry->scriptRegistry.clear();
        }

        m_NativeScriptRegistry = nullptr;
        m_InitializeFunction = nullptr;

        if (FreeLibraryFunc(m_GameLibraryHandle) != 0) {
            const char* error = LibError();
            if (error) {
                DM_CORE_WARN("FAILED TO UNLOAD GAME LIBRARY: {}", error);
            }
        }

        m_GameLibraryHandle = nullptr;
    }
}

}
