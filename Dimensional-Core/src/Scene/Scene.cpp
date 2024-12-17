#include "Core/Application.hpp"
#include "EngineAPI.hpp"
#include "Log/log.hpp"
#include "Rendering/EnvironmentMap.hpp"
#include "Rendering/Mesh.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include <Asset/AssetManager.hpp>
#include <Scene/Components.hpp>
#include <Scene/Entity.hpp>
#include <Scene/Scene.hpp>
#include <cstdint>

namespace Dimensional {

Scene::Scene()
{
}

Scene::~Scene()
{
    DM_CORE_INFO("Scene Deleted");
}

void Scene::onSceneRuntimeStart()
{
    // TODO: Probably a better spot to put this. Needs to happen on every reload
    Application::getApp().getScriptManager().updateComponentMemberData();

    auto& app = Application::getApp();
    auto& t = app.getScriptManager();
    t.reloadGameLibrary("Assets/Scripts/build/libGameApp.so");

    t.onSceneStart();
}

void Scene::updateSceneRuntime()
{
    auto& t = Application::getApp().getScriptManager();
    t.onSceneUpdate();
}

void Scene::onSceneRuntimeEnd()
{
    auto& t = Application::getApp().getScriptManager();
    t.onSceneEnd();
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
    auto& idComp = outEntity.getComponent<IDComponent>();
    idComp.ID = UUID();
    return outEntity;
}

void Scene::destroyEntity(Entity entity)
{

    if (entity.hasComponent<NativeScriptComponent>()) {
        auto& comp = entity.getComponent<NativeScriptComponent>();
        // Application::getApp().getScriptManager().m_NativeScriptRegistry->scriptRegistry[comp.className].classDestructor(comp.objectPointer);
    }

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
void Scene::copyComponentOnAllEntities(entt::registry& destReg, entt::registry& srcReg, const UMap<UUID, entt::entity>& eMap)
{
    auto srcEnts = srcReg.view<T>();
    for (auto& srcE : srcEnts) {
        entt::entity dstE = eMap.at(srcReg.get<IDComponent>(srcE).ID);
        auto& srcComp = srcReg.get<T>(srcE);
        destReg.emplace_or_replace<T>(dstE, srcComp);
    }
}

void Scene::deepCopy(Ref<Scene>& dest)
{
    UMap<UUID, entt::entity> newEntityMap;
    auto idComps = getAllEntitiesWith<IDComponent>();
    for (auto e : idComps) {
        UUID uid = m_Registry.get<IDComponent>(e).ID;
        std::string tag = m_Registry.get<TagComponent>(e).Tag;
        auto newEntity = dest->createEntityWithUUID(uid, tag);
        newEntityMap[uid] = newEntity.m_Handle;
    }

    copyAllEntitiesToNewReg(dest->m_Registry, m_Registry, newEntityMap, EveryComponentNoID {});
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
    component.radius = 1.0f;
}
template <>
void Scene::onComponentAdded<SpotLightComponent>(Entity entity, SpotLightComponent& component)
{
    component.intensity = 0.5f;
    component.radius = 1.0f;
}

template <>
void Scene::onComponentAdded<SkyLight>(Entity entity, SkyLight& component)
{
}

template <>
void Scene::onComponentAdded<DirectionalLightComponent>(Entity entity, DirectionalLightComponent& component)
{
}

template <>
void Scene::onComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
{
}
}
