#include "Scene/EntitySerializer.hpp"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "yaml-cpp/emitter.h"
#include "yaml-cpp/emittermanip.h"
#include <yaml-cpp/yaml.h>

namespace Dimensional {

void EntitySerialzer::Serialize(YAML::Emitter& out, Entity entity)
{
    DM_CORE_ASSERT(entity.hasComponent<IDComponent>(), "FAILED TO SERIALIZE ENTITY, ENTITY DOES NOT HAVE ID COMPONENT");

    out << YAML::BeginMap;
    out << YAML::Key << "Entity" << YAML::Value << entity.getID();

    if (entity.hasComponent<IDComponent>()) {
        auto& comp = entity.getComponent<IDComponent>();
        out << YAML::Key << "IDComponent";
        out << YAML::BeginMap;

        out << YAML::Key << "ID" << YAML::Value << comp.ID;

        out << YAML::EndMap;
    }

    if (entity.hasComponent<TagComponent>()) {
        auto& comp = entity.getComponent<TagComponent>();
        out << YAML::Key << "TagComponent";
        out << YAML::BeginMap;

        out << YAML::Key << "Tag" << YAML::Value << comp.Tag;

        out << YAML::EndMap;
    }

    if (entity.hasComponent<TransformComponent>()) {
        auto& comp = entity.getComponent<TransformComponent>();
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap;

        out << YAML::Key << "Position" << YAML::Value << comp.Position;
        out << YAML::Key << "Rotation" << YAML::Value << comp.Rotation;
        out << YAML::Key << "Scale" << YAML::Value << comp.Scale;

        out << YAML::EndMap;
    }

    if (entity.hasComponent<MeshRenderer>()) {
        auto& comp = entity.getComponent<MeshRenderer>();
        out << YAML::Key << "MeshRenderer";
        out << YAML::BeginMap;

        out << YAML::Key << "path" << YAML::Value << comp.model->path;

        out << YAML::EndMap;
    }

    if (entity.hasComponent<PointLightComponent>()) {
        auto& comp = entity.getComponent<PointLightComponent>();
        out << YAML::Key << "PointLightComponent";
        out << YAML::BeginMap;

        out << YAML::Key << "Color" << YAML::Value << comp.color;
        out << YAML::Key << "Intensity" << YAML::Value << comp.intensity;
        out << YAML::Key << "Constant" << YAML::Value << comp.constant;
        out << YAML::Key << "Linear" << YAML::Value << comp.linear;
        out << YAML::Key << "Quadratic" << YAML::Value << comp.quadratic;

        out << YAML::EndMap;
    }

    if (entity.hasComponent<SpotLightComponent>()) {
        auto& comp = entity.getComponent<SpotLightComponent>();
        out << YAML::Key << "SpotLightComponent";
        out << YAML::BeginMap;

        out << YAML::Key << "Color" << YAML::Value << comp.color;

        out << YAML::Key << "CutOff" << YAML::Value << comp.cutOff;
        out << YAML::Key << "OuterCutOff" << YAML::Value << comp.outerCutOff;

        out << YAML::Key << "Intensity" << YAML::Value << comp.intensity;
        out << YAML::Key << "Constant" << YAML::Value << comp.constant;
        out << YAML::Key << "Linear" << YAML::Value << comp.linear;
        out << YAML::Key << "Quadratic" << YAML::Value << comp.quadratic;

        out << YAML::EndMap;
    }
    out << YAML::EndMap;
}

}
