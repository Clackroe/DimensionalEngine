#include "Core/Application.hpp"
#include "EngineAPI.hpp"
#include "Log/log.hpp"
#include "Scene/Components.hpp"
#include <Scripting/NativeScriptManager.hpp>
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

    for (auto& [k, v] : *m_NativeScriptRegistry) {
        DM_CORE_INFO("Script: {}", v.className);
        for (auto& m : v.memberData) {
            DM_CORE_INFO("Member: {0}", m.varName);
        }
        m_Instances.push_back(new ScriptInstance(v, 0));
    }

    DM_CORE_WARN("SIZE: {}", m_Instances.size());

    std::string t = "speed";
    DM_CORE_WARN("SPEED: {}", m_Instances[0]->getData<float>(t));

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
