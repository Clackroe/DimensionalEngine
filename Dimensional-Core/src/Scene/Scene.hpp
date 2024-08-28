#ifndef DM_SCENE_H
#define DM_SCENE_H
#include <Core/UUID.hpp>
#include <core.hpp>

#include <entt/entt.hpp>

namespace Dimensional {
class Entity;

class Scene {
public:
    Scene();
    ~Scene();

    void updateEditor();

    Entity createEntity(const std::string& name = std::string());
    Entity createEntityWithUUID(UUID uuid, const std::string& name = std::string());

    void destroyEntity(Entity entity);

    Entity getEntityByName(std::string_view name);
    Entity getEntityByID(UUID id);

    template <typename... Components>
    auto getAllEntitiesWith() { return m_Registry.view<Components...>(); }

private:
    template <typename T>
    void onComponentAdded(Entity entity, T& component);

    entt::registry m_Registry;

    UMap<UUID, entt::entity> m_EntityMap;

    friend class Entity;
};

}
#endif
