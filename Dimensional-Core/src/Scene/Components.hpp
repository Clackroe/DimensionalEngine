#ifndef DM_COMPONENTS_H
#define DM_COMPONENTS_H
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
struct IDComponent {
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

struct TransformComponent {
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

struct MeshRenderer {
    MeshRenderer() = default;
    MeshRenderer(const MeshRenderer&) = default;
    MeshRenderer(const Ref<Model> modelIn)
    {
        model = modelIn;
        mat = CreateRef<Material>();
    }
    MeshRenderer(const Ref<Model> modelIn, const Ref<Material> matIn)
    {
        model = modelIn;
        mat = matIn;
    }

    Ref<Model> model;
    Ref<Material> mat;
};

struct PointLightComponent {
    PointLightComponent() = default;
    PointLightComponent(const PointLightComponent&) = default;

    glm::vec3 color = glm::vec3(1.0f);

    float intensity;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLightComponent {
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
