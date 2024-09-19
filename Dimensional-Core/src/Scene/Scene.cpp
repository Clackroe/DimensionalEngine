#include "Log/log.hpp"
#include "Rendering/Mesh.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include <Core/Assets/AssetManager.hpp>
#include <Scene/Components.hpp>
#include <Scene/Entity.hpp>
#include <Scene/Scene.hpp>
namespace Dimensional {

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::beginScene()
{
    // Submit Lights
    {
        auto view = m_Registry.view<TransformComponent, PointLightComponent>();
        for (auto e : view) {
            auto [transform, light] = view.get<TransformComponent, PointLightComponent>(e);
            LightData data = {
                transform.Position,
                transform.Rotation,
                light.color * 255.0f,
                0.0f,
                0.0f,
                light.intensity,
                light.constant,
                light.linear,
                light.quadratic
            };
            Renderer::submitLight(data);
        }
    }
    {
        auto view = m_Registry.view<TransformComponent, SpotLightComponent>();
        for (auto e : view) {
            auto [transform, light] = view.get<TransformComponent, SpotLightComponent>(e);

            glm::vec3 spotlightDirection = glm::rotate(glm::quat(transform.Rotation), glm::vec3(0.0f, -1.0f, 0.0f));

            LightData data = {
                transform.Position,
                spotlightDirection,
                light.color * 255.0f,
                glm::cos(glm::radians(light.cutOff)),
                glm::cos(glm::radians(light.outerCutOff)),
                light.intensity,
                light.constant,
                light.linear,
                light.quadratic
            };
            Renderer::submitLight(data);
        }
    }
}

void Scene::updateEditor()
{

    // Render Lights | Should possibly be done in another spot.
    {
        auto view = m_Registry.view<TransformComponent, PointLightComponent>();
        for (auto e : view) {
            auto [transform, light] = view.get<TransformComponent, PointLightComponent>(e);

            Ref<Material> mat = AssetManager::getMaterial("Default");
            Renderer::renderCube(mat, transform.GetTransform());
        }
    }

    {
        auto view = m_Registry.view<TransformComponent, SpotLightComponent>();
        for (auto e : view) {
            auto [transform, light] = view.get<TransformComponent, SpotLightComponent>(e);

            Ref<Material> mat = AssetManager::getMaterial("Default");
            Renderer::renderCube(mat, transform.GetTransform());
        }
    }

    {
        // Render Meshes
        auto view = m_Registry.view<TransformComponent, MeshRenderer>();
        for (auto e : view) {
            auto [transform, mesh] = view.get<TransformComponent, MeshRenderer>(e);
            Renderer::renderModel(*mesh.model, transform.GetTransform());
        }
    }
}

Entity Scene::createEntity(const std::string& name)
{
    return createEntityWithUUID(UUID(), name);
}

Entity Scene::createEntityWithUUID(UUID uuid, const std::string& name)
{
    Entity e = Entity(m_Registry.create(), this);
    e.addComponent<TransformComponent>();
    IDComponent id;
    id.ID = uuid;
    e.addComponent<IDComponent>(id);
    TagComponent tag = TagComponent();
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
void Scene::onComponentAdded<MeshRenderer>(Entity entity, MeshRenderer& component)
{
}
template <>
void Scene::onComponentAdded<TagComponent>(Entity entity, TagComponent& component)
{
}
template <>
void Scene::onComponentAdded<PointLightComponent>(Entity entity, PointLightComponent& component)
{
}
template <>
void Scene::onComponentAdded<SpotLightComponent>(Entity entity, SpotLightComponent& component)
{
}

}
