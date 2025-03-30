#ifndef DM_ENTITY_H
#define DM_ENTITY_H
#include <Core/UUID.hpp>
#include <Scene/Components.hpp>
#include <Scene/Scene.hpp>

#include <core.hpp>
#include <entt/entt.hpp>

namespace Dimensional {
class DMCORE_API Entity {
public:
    Entity() = default;
    Entity(entt::entity eHandle, Scene* scene)
        : m_Handle(eHandle)
        , m_Scene(scene)
    {
    }
    Entity(const Entity& otherEntity) = default;

    template <typename T, typename... Args>
    T& addComponent(Args&&... args)
    {
        DM_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");
        T& component = m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
        m_Scene->onComponentAdded<T>(*this, component);
        return component;
    }

    template <typename T, typename... Args>
    T& addOrReplaceComponent(Args&&... args)
    {
        T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_Handle, std::forward<Args>(args)...);
        m_Scene->onComponentAdded<T>(*this, component);
        return component;
    }

    template <typename T>
    T& getComponent()
    {
        DM_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
        return m_Scene->m_Registry.get<T>(m_Handle);
    }

    template <typename T>
    bool hasComponent()
    {
        return m_Scene->m_Registry.all_of<T>(m_Handle);
    }

    template <typename T>
    void removeComponent()
    {
        DM_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
        m_Scene->m_Registry.remove<T>(m_Handle);
    }

    // ----
    UUID getID() { return getComponent<IDComponent>().ID; };

    // ---- Operators ----
    operator bool() const { return m_Handle != entt::null; }
    operator entt::entity() const { return m_Handle; }
    operator u32() const { return (u32)m_Handle; }

    bool operator==(const Entity& other) const
    {
        return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
    }
    bool operator!=(const Entity& other) const
    {
        return !(*this == other);
    }

private:
    entt::entity m_Handle { entt::null };
    Scene* m_Scene = nullptr;

    friend class Scene;
};
}

#endif
