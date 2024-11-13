#include "Log/log.hpp"
#include "Scripting/include/EngineAPI.hpp"
#include <Scripting/NativeScriptManager.hpp>
namespace Dimensional {
NativeScriptManager::NativeScriptManager()
{
}
NativeScriptManager::~NativeScriptManager()
{
}

void NativeScriptManager::reloadGameLibrary(const std::string& path)
{
    gameLibraryHandle = LoadLibraryFunc(path.c_str());
    if (!gameLibraryHandle) {
        DM_CORE_WARN("FAILED TO LOAD GAME LIBRARY");
    }
    bool success = loadLibraryFunction(gameLibraryHandle, "Init", initializeFunction);
    if (!success) {
        DM_CORE_WARN("Cannot retrieve initializeFunction from game library");
        gameLibraryHandle = nullptr;
        return;
    }
    EngineAPI* api = getEngineAPI();
    ScriptableEntityData* t = initializeFunction(api);
    t->onUpdate();
}

}
