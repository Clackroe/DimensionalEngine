#ifndef DM_SC_COMPONANTS
#define DM_SC_COMPONANTS
#include "EngineAPI.hpp"
#include "NativeScripting.hpp"
namespace ScriptingCore {

class Component {
public:
    Component() = default;
    Component(ComponentHandle* handle)
        : m_Handle(handle)
        , cAPI(ScriptCoreLink::getComponentAPI())
    {
    }

protected:
    ComponentHandle* m_Handle;
    ComponentAPI* cAPI;
};

class Transform : public Component {
public:
    Transform() = default;
    Transform(TransformCompHandle* handle)
        : Component(handle)
    {
    }

    glm::vec3 getPosition()
    {
        return cAPI->Transform_GetPosition(m_Handle);
    }

    void setPosition(glm::vec3 position)
    {
        cAPI->Transform_SetPosition(m_Handle, position);
    }

    glm::vec3 getRotationRadians()
    {
        return cAPI->Transform_GetRotationRadians(m_Handle);
    };
    glm::vec3 getRotationDegrees()
    {
        return cAPI->Transform_GetRotationDegrees(m_Handle);
    };
    void setRotationRadians(glm::vec3 rot)
    {
        cAPI->Transform_SetRotationRadians(m_Handle, rot);
    };
    void setRotationDegrees(glm::vec3 rot)
    {
        cAPI->Transform_SetRotationDegrees(m_Handle, rot);
    };

    glm::vec3 getScale()
    {
        return cAPI->Transform_GetScale(m_Handle);
    };
    void setScale(glm::vec3 scale)
    {
        cAPI->Transform_SetScale(m_Handle, scale);
    };

    static Transform getComponent(uint64_t eHandle)
    {
        return Transform(ScriptCoreLink::getComponentAPI()->Transform_GetComp(eHandle));
    }
};

}

#endif
