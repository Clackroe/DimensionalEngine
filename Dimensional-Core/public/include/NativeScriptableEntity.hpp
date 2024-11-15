#ifndef DM_NATIVE_SCRIPTABLE_ENTITY
#define DM_NATIVE_SCRIPTABLE_ENTITY
#include "EngineAPI.hpp"
#include <iostream>
#include <string>

extern EngineAPI* g_EngineAPI;

class NativeScriptableEntity {
public:
    NativeScriptableEntity(uint64_t entityHandle)
        : m_EntityHandle(entityHandle)
    {
    }

    template <typename T>
    T GetComponent() {
    };

    ~NativeScriptableEntity() = default;

protected:
    uint64_t m_EntityHandle = 0;
};
#endif
