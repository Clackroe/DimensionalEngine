#include "Scene/Components.hpp"
#include <Scene/Entity.hpp>
#include <Scene/Scene.hpp>
namespace Dimensional {

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::updateEditor()
{
    // Render Meshes
    // auto view = m_Registry.view<TransformComponent, MeshRenderer>();
    // for (auto e : view) {
    //     auto [transform, mesh] = view.get<TransformComponent, MeshRenderer>(e);
    //
    //
    // }
}

Entity Scene::createEntity(const std::string& name)
{
    return createEntityWithUUID(UUID(), name);
}

Entity Scene::createEntityWithUUID(UUID uuid, const std::string& name)
{
    Entity e = { m_Registry.create(), this };
    e.addComponent<TransformComponent>();
    IDComponent id;
    id.ID = uuid;
    e.addComponent<IDComponent>(id);
    TagComponent tag = e.addComponent<TagComponent>();
    tag.Tag = name.empty() ? "Unamed Entity" : name;
    e.addComponent<TagComponent>(tag);
    m_EntityMap[uuid] = e;
    return e;
}

void Scene::destroyEntity(Entity entity)
{
    m_EntityMap.erase(entity.getID());
    m_Registry.destroy(entity);
}

Entity Scene::getEntityByName(std::string_view name)
{
    auto v = m_Registry.view<TagComponent>();
    for (auto e : v) {
        const TagComponent& tag = v.get<TagComponent>(e);
        if (tag.Tag == name) {
            return Entity { e, this };
        }
    }
    return {};
}
Entity Scene::getEntityByID(UUID id)
{
    if (m_EntityMap.find(id) != m_EntityMap.end())
        return { m_EntityMap.at(id), this };

    return {};
}

template <typename T>
void Scene::onComponentAdded(Entity entity, T& component)
{
    DM_CORE_ASSERT(sizeof(T) != 0, "NULL Component attempted to add!");
}
template <>
void Scene::onComponentAdded<IDComponent>(Entity entity, IDComponent& component)
{
}

template <>
void Scene::onComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
{
}

template <>
void Scene::onComponentAdded<TagComponent>(Entity entity, TagComponent& component)
{
}

}
