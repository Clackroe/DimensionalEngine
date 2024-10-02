#include "Log/log.hpp"
#include "Rendering/EnvironmentMap.hpp"
#include "Rendering/Mesh.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include <Assets/AssetManager.hpp>
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

    // Submit Environment Data
    {
        auto view = m_Registry.view<SkyLight>();
        for (auto l : view) {
            auto envData = view.get<SkyLight>(l);
            Ref<EnvironmentMap> map = AssetManager::getInstance().getAsset<EnvironmentMap>(envData.envMap);
            if (map) {
                Renderer::submitEnvironment({ map, envData.lod });
                return;
            }
        }
        // Reset if no skylight exists
        Renderer::submitEnvironment({ nullptr });
    }
}

void Scene::updateEditor()
{

    // Render Lights | Should possibly be done in another spot.
    {
        auto view = m_Registry.view<TransformComponent, PointLightComponent>();
        for (auto e : view) {
            auto [transform, light] = view.get<TransformComponent, PointLightComponent>(e);

            // TODO: Render Billboard sprite

            // TEMPORARY FOR NOW
            static Ref<Material> mat;
            if (!mat) {
                mat = CreateRef<Material>();
            }
            Renderer::renderCube(mat, transform.GetTransform());
        }
    }

    {
        auto view = m_Registry.view<TransformComponent, SpotLightComponent>();
        for (auto e : view) {
            auto [transform, light] = view.get<TransformComponent, SpotLightComponent>(e);

            // TODO: Render Billboard sprite

            // TEMPORARY FOR NOW
            static Ref<Material> mat;
            if (!mat) {
                mat = CreateRef<Material>();
            }
            Renderer::renderCube(mat, transform.GetTransform());
        }
    }

    {
        // Render Meshes
        auto view = m_Registry.view<TransformComponent, MeshRenderer>();
        for (auto e : view) {
            auto [transform, mesh] = view.get<TransformComponent, MeshRenderer>(e);
            Ref<Model> mod = AssetManager::getInstance().getAsset<Model>(mesh.model);
            if (!mod) {
                continue;
            }
            Renderer::renderModel(*mod, transform.GetTransform(), mesh.materialOverrides);
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

// Thanks Cherno, Template magic right here
template <typename... Component>
static void CopyComponentIfExists(Entity dst, Entity src)
{
    ([&]() {
        if (src.hasComponent<Component>())
            dst.addOrReplaceComponent<Component>(src.getComponent<Component>());
    }(),
        ...);
}

template <typename... Component>
static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
{
    CopyComponentIfExists<Component...>(dst, src);
}
Entity Scene::duplicateEntity(Entity e)
{
    std::string name = e.getComponent<TagComponent>().Tag;
    Entity outEntity = createEntity(name);
    CopyComponentIfExists(EveryComponent {}, outEntity, e);
    return outEntity;
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
    component.intensity = 0.5f;
    component.constant = 6.0f;
}
template <>
void Scene::onComponentAdded<SpotLightComponent>(Entity entity, SpotLightComponent& component)
{
    component.intensity = 0.5f;
    component.constant = 6.0f;
}

template <>
void Scene::onComponentAdded<SkyLight>(Entity entity, SkyLight& component)
{
}

}
