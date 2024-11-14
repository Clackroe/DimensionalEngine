#ifndef DM_NATIVE_SCRIPTABLE_ENTITY
#define DM_NATIVE_SCRIPTABLE_ENTITY
#include "EngineAPI.hpp"

extern EngineAPI* g_EngineAPI;

class NativeScriptableEntity {
public:
    NativeScriptableEntity(uint64_t entityHandle)
        : m_EntityHandle(entityHandle)
    {
    }
    ~NativeScriptableEntity() = default;

private:
    uint64_t m_EntityHandle = 0;
};
#endif
