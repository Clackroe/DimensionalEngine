// #include "yaml-cpp/emitter.h"
// #include "yaml-cpp/emittermanip.h"
// #include <Asset/MaterialSerializer.hpp>
// #include <yaml-cpp/yaml.h>
//
// namespace YAML {
// static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
// {
//     out << YAML::Flow;
//     out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
//     return out;
// }
//
// template <>
// struct convert<glm::vec3> {
//     static Node encode(const glm::vec3& rhs)
//     {
//         Node node;
//         node.push_back(rhs.x);
//         node.push_back(rhs.y);
//         node.push_back(rhs.z);
//         return node;
//     }
//
//     static bool decode(const Node& node, glm::vec3& rhs)
//     {
//         if (!node.IsSequence() || node.size() != 3)
//             return false;
//
//         return convert<float>::decode(node[0], rhs.x)
//             && convert<float>::decode(node[1], rhs.y)
//             && convert<float>::decode(node[2], rhs.z);
//     }
// };
// }
// namespace Dimensional {
// void MaterialSerializer::Serialize(std::filesystem::path savePath, MaterialSettings& material)
// {
//     YAML::Emitter out;
//     out << YAML::BeginMap;
//     out << YAML::Key << "Material" << YAML::Value << YAML::BeginMap;
//
//     out << YAML::Key << "Albedo" << YAML::Value << material.Albedo;
//     out << YAML::Key << "Normal" << YAML::Value << material.Normal;
//     out << YAML::Key << "Metal" << YAML::Value << material.Metalness;
//     out << YAML::Key << "Roughness" << YAML::Value << material.Roughness;
//     out << YAML::Key << "AO" << YAML::Value << material.AO;
//
//     out << YAML::Key << "UseMetalMap" << YAML::Value << material.useMetalMap;
//     out << YAML::Key << "UseRoughnessMap" << YAML::Value << material.useRoughnessMap;
//     out << YAML::Key << "Color" << YAML::Value << material.color;
//     out << YAML::Key << "MetalnessMultiplier" << YAML::Value << material.metalnessMult;
//     out << YAML::Key << "RoughnessMultiplier" << YAML::Value << material.roughnessMult;
//
//     out << YAML::EndMap;
//     out << YAML::EndMap;
//
//     std::ofstream fout(savePath.c_str());
//     fout << out.c_str();
// }
//
// void MaterialSerializer::Deserialize(std::filesystem::path file, MaterialSettings& settings)
// {
//
//     YAML::Node rootNode = YAML::LoadFile(file.string());
//
//     auto mat = rootNode["Material"];
//
//     settings.Albedo = mat["Albedo"].as<u64>();
//     settings.Normal = mat["Normal"].as<u64>();
//     settings.Metalness = mat["Metal"].as<u64>();
//     settings.Roughness = mat["Roughness"].as<u64>();
//     settings.AO = mat["AO"].as<u64>();
//
//     settings.useMetalMap = mat["UseMetalMap"].as<bool>();
//     settings.useRoughnessMap = mat["UseRoughnessMap"].as<bool>();
//     settings.color = mat["Color"].as<glm::vec3>();
//     settings.metalnessMult = mat["MetalnessMultiplier"].as<float>();
//     settings.roughnessMult = mat["RoughnessMultiplier"].as<float>();
// }
// }
