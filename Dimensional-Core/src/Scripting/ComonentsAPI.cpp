#include "Core/Application.hpp"
#include "Log/log.hpp"
#include "core.hpp"
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

    glm::vec3 getPosition(TransformCompHandle* handle)
    {
        TransformComponent* tc = reinterpret_cast<TransformComponent*>(handle);
        DM_CORE_ASSERT(tc, "Transform Component Is Null");
        glm::vec3 p = tc->Position;
        return tc->Position;
    }

    void setPosition(TransformCompHandle* handle, glm::vec3 position)
    {
        TransformComponent* tc = reinterpret_cast<TransformComponent*>(handle);
        DM_CORE_ASSERT(tc, "Transform Component Is Null");
        tc->Position = position;
    }

    ComponentAPI g_CompAPI {
        .Transform_GetComp = getComponent<TransformComponent, TransformCompHandle>,
        .Transform_GetPosition = getPosition,
        .Transform_SetPosition = setPosition,
    };

    extern "C" ComponentAPI* getComponentAPI()
    {
        return &g_CompAPI;
    }
}

}
