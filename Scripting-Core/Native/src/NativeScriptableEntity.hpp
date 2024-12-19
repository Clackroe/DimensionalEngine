#ifndef DM_NATIVE_SCRIPTABLE_ENTITY
#define DM_NATIVE_SCRIPTABLE_ENTITY
#include "EngineAPI.hpp"
#include <Components.hpp>
#include <iostream>
#include <string>

namespace ScriptingCore {

class NativeScriptableEntity {
public:
    NativeScriptableEntity(uint64_t entityHandle)
        : m_EntityHandle(entityHandle)
    {
        transform = Transform::getComponent(m_EntityHandle);
    }

    template <typename T>
    T GetComponent()
    {
        // TODO: Ensure the component provided has this function.
        return T::getComponent(m_EntityHandle);
    };

    ~NativeScriptableEntity() = default;

protected:
    uint64_t m_EntityHandle = 0;
    Transform transform;
};
}
#endif
