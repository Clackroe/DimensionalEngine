#ifndef DM_NATIVE_SCRIPTABLE_ENTITY
#define DM_NATIVE_SCRIPTABLE_ENTITY
#include "Scripting/include/EngineAPI.hpp"

extern EngineAPI* g_EngineAPI;

class NativeScriptableEntity {
    NativeScriptableEntity(int entityHandle)
        : m_EntityHandle(entityHandle)
    {
    }
    ~NativeScriptableEntity() = default;

private:
    int m_EntityHandle = 0;
};
#endif
