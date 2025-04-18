#include "Scene/EntitySerializer.hpp"
#include "Asset/AssetManager.hpp"
#include "Core/Application.hpp"
#include "EngineAPI.hpp"
#include "Log/log.hpp"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "Scripting/NativeScriptManager.hpp"
#include "yaml-cpp/binary.h"
#include "yaml-cpp/emitter.h"
#include "yaml-cpp/emittermanip.h"
#include "yaml-cpp/node/detail/iterator_fwd.h"
#include <cstring>
#include <yaml-cpp/yaml.h>

namespace YAML {
YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}

template <>
struct convert<glm::vec3> {
    static Node encode(const glm::vec3& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        return node;
    }

    static bool decode(const Node& node, glm::vec3& rhs)
    {
        if (!node.IsSequence() || node.size() != 3)
            return false;

        return convert<float>::decode(node[0], rhs.x)
            && convert<float>::decode(node[1], rhs.y)
            && convert<float>::decode(node[2], rhs.z);
    }
};

template <>
struct convert<Dimensional::UUID> {
    static Node encode(const Dimensional::UUID& uuid)
    {
        Node node;
        node.push_back(static_cast<u64>(uuid));
        return node;
    }

    static bool decode(const Node& node, Dimensional::UUID& uuid)
    {
        u64 tmp = 0;
        bool success = convert<u64>::decode(node, tmp);
        if (success)
            uuid = tmp;
        return success;
    }
};
}

namespace Dimensional {

template <typename T>
inline T SetValue(T& value, const YAML::Node& node)
{
    if (node)
        value = node.as<T>(value);
    return value;
}

void EntitySerialzer::Serialize(YAML::Emitter& out, Entity entity)
{
    DM_CORE_ASSERT(entity.hasComponent<IDComponent>(), "FAILED TO SERIALIZE ENTITY, ENTITY DOES NOT HAVE ID COMPONENT");

    out << YAML::BeginMap;
    out << YAML::Key << "Entity" << YAML::Value << entity.getID();

    UUID id = -1;

    if (entity.hasComponent<IDComponent>()) {
        auto& comp = entity.getComponent<IDComponent>();
        out << YAML::Key << "IDComponent";
        out << YAML::BeginMap;

        out << YAML::Key << "ID" << YAML::Value << comp.ID;
        id = comp.ID;

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

        out << YAML::Key << "modelHandle" << YAML::Value << (u64)comp.model;
        out << YAML::Key << "materialOverrides" << YAML::Value;
        out << YAML::BeginSeq;
        for (auto id : comp.materialOverrides) {
            out << (u64)id;
        }

        out << YAML::EndSeq;

        out << YAML::EndMap;
    }

    if (entity.hasComponent<SkyLight>()) {
        auto& comp = entity.getComponent<SkyLight>();
        out << YAML::Key << "SkyLight";
        out << YAML::BeginMap;
        out << YAML::Key << "environmentMap" << YAML::Value << (u64)comp.envMap;
        out << YAML::Key << "environmentLOD" << YAML::Value << comp.lod;
        out << YAML::EndMap;
    }

    if (entity.hasComponent<PointLightComponent>()) {
        auto& comp = entity.getComponent<PointLightComponent>();
        out << YAML::Key << "PointLightComponent";
        out << YAML::BeginMap;

        out << YAML::Key << "Color" << YAML::Value << comp.color;
        out << YAML::Key << "Intensity" << YAML::Value << comp.intensity;
        out << YAML::Key << "Radius" << YAML::Value << comp.radius;

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
        out << YAML::Key << "Radius" << YAML::Value << comp.radius;

        out << YAML::EndMap;
    }

    if (entity.hasComponent<DirectionalLightComponent>()) {
        auto& comp = entity.getComponent<DirectionalLightComponent>();
        out << YAML::Key << "DirectionalLightComponent";
        out << YAML::BeginMap;

        out << YAML::Key << "Color" << YAML::Value << comp.color;
        out << YAML::Key << "Intensity" << YAML::Value << comp.intensity;

        out << YAML::EndMap;
    }

    if (entity.hasComponent<NativeScriptComponent>()) {
        auto& comp = entity.getComponent<NativeScriptComponent>();
        out << YAML::Key << "NativeScriptComponent";
        out << YAML::BeginMap;

        out << YAML::Key << "ClassName" << YAML::Value << comp.className;

        NativeScriptManager& manager = Application::getApp().getScriptManager();

        if (manager.m_ComponentMembers.contains(id)) {

            out << YAML::Key << "ExposedVariables" << YAML::BeginMap;

            auto& members = manager.m_ComponentMembers.at(id);
            for (auto& [name, data] : members) {
                out << YAML::Key << name << YAML::BeginMap;

                out << YAML::Key << "DataType" << YAML::Value << g_ScriptMemberToString[data.dataType];
                out << YAML::Key << "Name" << YAML::Value << data.name;
                out << YAML::Key << "IsChanged" << YAML::Value << data.dataChanged;
                out << YAML::Key << "DataSize" << YAML::Value << data.sizeBytes;
                out << YAML::Key << "DataRawBytes" << YAML::Value << YAML::Binary(data.data, data.sizeBytes);

                out << YAML::EndMap;
            }

            out << YAML::EndMap;
        } else {
            DM_CORE_WARN("Tried to serialize Script Component that doesn't exist: {}", comp.className)
        }

        out << YAML::EndMap;
    }

    out << YAML::EndMap;
}

UUID EntitySerialzer::Deserialize(const YAML::Node& node, Ref<Scene>& scene)
{

    UUID id;
    auto entityIdSection = node["Entity"];
    SetValue(id, entityIdSection);
    Entity loadedEntity = scene->createEntityWithUUID(id);

    std::string name;
    auto tagComp = node["TagComponent"];
    if (tagComp) {
        SetValue(name, tagComp["Tag"]);
        auto& tc = loadedEntity.getComponent<TagComponent>();
        tc.Tag = name;
    }

    auto transComp = node["TransformComponent"];
    if (transComp) {

        auto& tc = loadedEntity.addOrReplaceComponent<TransformComponent>();
        SetValue(tc.Position, transComp["Position"]);
        SetValue(tc.Rotation, transComp["Rotation"]);
        SetValue(tc.Scale, transComp["Scale"]);
    }

    auto meshComp = node["MeshRenderer"];
    if (meshComp) {
        AssetHandle handle = 0;
        SetValue(handle, meshComp["modelHandle"]);
        auto& comp = loadedEntity.addComponent<MeshRenderer>(handle);

        comp.materialOverrides.clear();
        for (auto node : meshComp["materialOverrides"]) {
            comp.materialOverrides.push_back(node.as<u64>());
        }
    }

    auto skyLight = node["SkyLight"];
    if (skyLight) {
        auto& skComp = loadedEntity.addComponent<SkyLight>();
        SetValue(skComp.envMap, skyLight["environmentMap"]);
        SetValue(skComp.lod, skyLight["environmentLOD"]);
    }

    auto pointComp = node["PointLightComponent"];
    if (pointComp) {

        auto& pComp = loadedEntity.addComponent<PointLightComponent>();

        SetValue(pComp.color, pointComp["Color"]);
        SetValue(pComp.intensity, pointComp["Intensity"]);
        SetValue(pComp.radius, pointComp["Radius"]);
    }

    auto spotComp = node["SpotLightComponent"];
    if (spotComp) {

        auto& sComp = loadedEntity.addComponent<SpotLightComponent>();

        SetValue(sComp.color, spotComp["Color"]);

        SetValue(sComp.cutOff, spotComp["CutOff"]);
        SetValue(sComp.outerCutOff, spotComp["OuterCutOff"]);

        SetValue(sComp.intensity, spotComp["Intensity"]);
        SetValue(sComp.radius, spotComp["Radius"]);
    }

    auto dirComp = node["DirectionalLightComponent"];
    if (dirComp) {

        auto& dComp = loadedEntity.addComponent<DirectionalLightComponent>();

        SetValue(dComp.color, dirComp["Color"]);

        SetValue(dComp.intensity, dirComp["Intensity"]);
    }

    /*
 *         out << YAML::Key << "NativeScriptComponent";
        out << YAML::BeginMap;

        out << YAML::Key << "ClassName" << YAML::Value << comp.className;

        NativeScriptManager& manager = Application::getApp().getScriptManager();

            out << YAML::Key << "ExposedVariables" << YAML::BeginMap;

            auto& members = manager.m_ComponentMembers.at(id);
            for (auto& [name, data] : members) {
                out << YAML::Key << name << YAML::BeginMap;

                out << YAML::Key << "DataType" << YAML::Value << g_ScriptMemberToString[data.dataType];
                out << YAML::Key << "Name" << YAML::Value << data.name;
                out << YAML::Key << "IsChanged" << YAML::Value << data.dataChanged;
                out << YAML::Key << "DataSize" << YAML::Value << data.sizeBytes;


                out << YAML::Key << "DataRawBytes" << YAML::Value << YAML::Binary(data.data, data.sizeBytes);

                out << YAML::EndMap;
            }

            out << YAML::EndMap;

        out << YAML::EndMap;

     * */

    auto nsComp = node["NativeScriptComponent"];
    if (nsComp) {

        auto& comp = loadedEntity.addComponent<NativeScriptComponent>();
        SetValue(comp.className, nsComp["ClassName"]);

        NativeScriptManager& manager = Application::getApp().getScriptManager();

        UMap<std::string, ScriptComponentMember> members;

        auto membersYaml = nsComp["ExposedVariables"];
        for (YAML::const_iterator it = membersYaml.begin(); it != membersYaml.end(); ++it) {
            std::string name = it->first.as<std::string>();
            auto yam = it->second;

            //             out << YAML::Key << "DataType" << YAML::Value << g_ScriptMemberToString[data.dataType];
            // out << YAML::Key << "Name" << YAML::Value << data.name;
            // out << YAML::Key << "IsChanged" << YAML::Value << data.dataChanged;
            // out << YAML::Key << "DataSize" << YAML::Value << data.sizeBytes;

            ScriptComponentMember member;
            SetValue(member.name, yam["Name"]);
            std::string typeString;
            SetValue(typeString, yam["DataType"]);
            member.dataType = g_StringToScriptMember[typeString];
            SetValue(member.dataChanged, yam["IsChanged"]);
            SetValue(member.sizeBytes, yam["DataSize"]);

            YAML::Binary bin = yam["DataRawBytes"].as<YAML::Binary>();
            memcpy(member.data, bin.data(), member.sizeBytes);

            members[name] = member;
        }

        manager.m_ComponentMembers[id] = members;
    }

    return loadedEntity.getID();
}

}
