#include "Log/log.hpp"
#include "Scripting/include/EngineAPI.hpp"
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
        const char* error = dlerror();
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
        return;
    }
    EngineAPI* api = getEngineAPI();
    NativeScriptRegistry* tempReg = m_InitializeFunction(api);
    if (tempReg) {
        m_NativeScriptRegistry = tempReg;
    } else {
        DM_CORE_WARN("Failed to retrieve reflection data for Native Script");
        return;
    }
}

void NativeScriptManager::freeGameLibrary()
{
    if (m_GameLibraryHandle) {
        m_InitializeFunction = nullptr; // Clear function pointer
        m_NativeScriptRegistry->scriptRegistry.clear(); // Clear any registry references

        if (dlclose(m_GameLibraryHandle) != 0) { // Ensure successful unload
            const char* error = dlerror();
            if (error) {
                DM_CORE_WARN("FAILED TO UNLOAD GAME LIBRARY: {}", error);
            }
        }

        m_GameLibraryHandle = nullptr; // Nullify handle to avoid accidental reuse
    }
}

}
