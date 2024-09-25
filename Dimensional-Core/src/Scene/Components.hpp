#ifndef DM_COMPONENTS_H
#define DM_COMPONENTS_H
#include "Assets/AssetManager.hpp"
#include <Core/UUID.hpp>
#include <core.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Rendering/Material.hpp>
#include <Rendering/Model.hpp>
#include <Rendering/Renderer.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Dimensional {
struct DMCORE_API IDComponent {
    UUID ID;

    IDComponent() = default;
    IDComponent(const IDComponent&) = default;
};

struct TagComponent {
    std::string Tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const ::std::string& tag)
        : Tag(tag)
    {
    }
};

struct DMCORE_API TransformComponent {
    glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& translation)
        : Position(translation)
    {
    }

    glm::mat4 GetTransform() const
    {
        glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
        return glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scale);
    }
};

struct DMCORE_API MeshRenderer {
    MeshRenderer() = default;
    MeshRenderer(const MeshRenderer&) = default;
    MeshRenderer(AssetHandle handle)
    {
        setModelHandle(handle);
    }

    void setModelHandle(AssetHandle newHandle)
    {
        if (model == newHandle) {
            return;
        }
        model = newHandle;
        Ref<Model> modelObj = AssetManager::getInstance().getAsset<Model>(model);
        if (!modelObj) {
            model = 0;
            return;
        }
        Ref<ModelSource> source = AssetManager::getInstance().getAsset<ModelSource>(modelObj->getSource());
        if (source) {
            materialOverrides.clear();
            for (u32 i = 0; i < source->getMaterialHandles().size(); i++) {
                materialOverrides.push_back(0);
            }
            return;
        }
        model = 0;
        return;
    }

    AssetHandle model = 0;
    std::vector<AssetHandle> materialOverrides;
};

struct DMCORE_API SkyLight {
    SkyLight() = default;
    SkyLight(const SkyLight&) = default;

    AssetHandle envMap = 0;
};

struct DMCORE_API PointLightComponent {
    PointLightComponent() = default;
    PointLightComponent(const PointLightComponent&) = default;

    glm::vec3 color = glm::vec3(1.0f);

    float intensity;
    float constant;
    float linear;
    float quadratic;
};

struct DMCORE_API SpotLightComponent {
    SpotLightComponent() = default;
    SpotLightComponent(const SpotLightComponent&) = default;

    glm::vec3 color = glm::vec3(1.0f);
    float cutOff;
    float outerCutOff;

    float intensity;
    float constant;
    float linear;
    float quadratic;
};

template <typename... Component>
struct ComponentGroup {
};

using EveryComponent = ComponentGroup<IDComponent, TagComponent, TransformComponent, MeshRenderer, PointLightComponent, SpotLightComponent>;

}
#endif
