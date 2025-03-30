#include "Core/Application.hpp"
#include "Log/log.hpp"
#include "core.hpp"
#include "glm/trigonometric.hpp"
#include <EngineAPI.hpp>
#include <Scene/Components.hpp>
#include <Scene/Entity.hpp>
#include <cstdint>
#include <string>

namespace Dimensional {

namespace ComponantScriptAPI {

    template <typename Comp, typename Handle>
    Handle* getComponent(uint64_t entityID)
    {
        auto sc = Application::getApp().getSceneCTX();
        DM_CORE_ASSERT(sc, "Scene Is Null!!");
        Entity e = sc->getEntityByID(entityID);
        DM_CORE_ASSERT(e, "Entity Is Null!!");

        bool hasComp = e.hasComponent<Comp>();
        DM_CORE_ASSERT(hasComp, "Tried to retreive component that the entity doesn't have!");
        if (!hasComp) {
            return nullptr;
        }

        auto component = &e.getComponent<Comp>();
        Handle* out = reinterpret_cast<Handle*>(component);

        return out;
    }

    template <typename T, typename H>
    inline static T* getCompFromOpaqueHandle(H* handle)
    {
        T* tc = reinterpret_cast<T*>(handle);
        DM_CORE_ASSERT(tc, "Transform Component Is Null");
        return tc;
    }

    // ===============TRANSFROM COMPONENT==============

    glm::vec3 getPosition(TransformCompHandle* handle)
    {
        auto tc = getCompFromOpaqueHandle<TransformComponent>(handle);
        glm::vec3 p = tc->Position;
        return tc->Position;
    }

    void setPosition(TransformCompHandle* handle, glm::vec3 position)
    {
        auto tc = getCompFromOpaqueHandle<TransformComponent>(handle);
        tc->Position = position;
    }

    glm::vec3 getRotationRadians(TransformCompHandle* handle)
    {
        auto tc = getCompFromOpaqueHandle<TransformComponent>(handle);
        return tc->Rotation;
    };
    glm::vec3 getRotationDegrees(TransformCompHandle* handle)
    {
        auto tc = getCompFromOpaqueHandle<TransformComponent>(handle);
        return glm::degrees(tc->Rotation);
    };
    void setRotationRadians(TransformCompHandle* handle, glm::vec3 rot)
    {
        auto tc = getCompFromOpaqueHandle<TransformComponent>(handle);
        tc->Rotation = rot;
    };
    void setRotationDegrees(TransformCompHandle* handle, glm::vec3 rot)
    {
        auto tc = getCompFromOpaqueHandle<TransformComponent>(handle);
        tc->Rotation = glm::radians(rot);
    };

    glm::vec3 getScale(TransformCompHandle* handle)
    {
        auto tc = getCompFromOpaqueHandle<TransformComponent>(handle);
        return tc->Scale;
    };
    void setScale(TransformCompHandle* handle, glm::vec3 scale)
    {
        auto tc = getCompFromOpaqueHandle<TransformComponent>(handle);
        tc->Scale = scale;
    };

    ComponentAPI g_CompAPI {
        // Transform
        .Transform_GetComp = getComponent<TransformComponent, TransformCompHandle>,
        .Transform_GetPosition = getPosition,
        .Transform_SetPosition = setPosition,
        .Transform_GetRotationRadians = getRotationRadians,
        .Transform_GetRotationDegrees = getRotationDegrees,
        .Transform_SetRotationRadians = setRotationRadians,
        .Transform_SetRotationDegrees = setRotationDegrees,
        .Transform_GetScale = getScale,
        .Transform_SetScale = setScale
    };

    extern "C" ComponentAPI* getComponentAPI()
    {
        return &g_CompAPI;
    }
}

}
