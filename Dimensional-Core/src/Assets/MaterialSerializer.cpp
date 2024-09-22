#include "yaml-cpp/emitter.h"
#include "yaml-cpp/emittermanip.h"
#include <Assets/MaterialSerializer.hpp>
#include <yaml-cpp/yaml.h>

namespace Dimensional {
void MaterialSerializer::Serialize(std::filesystem::path savePath, MaterialSettings& material)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Material" << YAML::Value << YAML::BeginMap;

    out << YAML::Key << "Albedo" << YAML::Value << material.Albedo;
    out << YAML::Key << "Normal" << YAML::Value << material.Normal;
    out << YAML::Key << "Metal" << YAML::Value << material.Metalness;
    out << YAML::Key << "Roughness" << YAML::Value << material.Roughness;
    out << YAML::Key << "AO" << YAML::Value << material.AO;

    out << YAML::EndMap;
    out << YAML::EndMap;

    std::ofstream fout(savePath.c_str());
    fout << out.c_str();
}

void MaterialSerializer::Deserialize(std::filesystem::path file, MaterialSettings& settings)
{

    YAML::Node rootNode = YAML::LoadFile(file);

    auto mat = rootNode["Material"];

    settings.Albedo = mat["Albedo"].as<u64>();
    settings.Normal = mat["Normal"].as<u64>();
    settings.Metalness = mat["Metal"].as<u64>();
    settings.Roughness = mat["Roughness"].as<u64>();
    settings.AO = mat["AO"].as<u64>();
}
}
