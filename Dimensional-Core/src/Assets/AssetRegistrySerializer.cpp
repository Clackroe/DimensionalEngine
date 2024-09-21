#include "Assets/AssetRegistrySerializer.hpp"
#include "Log/log.hpp"
#include "yaml-cpp/emitter.h"
#include "yaml-cpp/emittermanip.h"
#include <yaml-cpp/yaml.h>
namespace Dimensional {
void AssetRegistrySerializer::Serialize(std::filesystem::path savePath, const AssetManager& manager)
{
    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Key << "AssetRegistry" << YAML::Value << YAML::BeginSeq;

    for (auto& [handle, meta] : manager.m_Registry) {
        out << YAML::BeginMap;
        out << YAML::Key << "handle" << YAML::Value << (u64)handle;
        out << YAML::Key << "sourcePath" << YAML::Value << meta.sourcePath;
        out << YAML::Key << "type" << YAML::Value << Asset::assetTypeToString(meta.type);
        out << YAML::EndMap;
    }

    out << YAML::EndSeq;

    std::ofstream fout(savePath.c_str());
    fout << out.c_str();
    DM_CORE_INFO("Saving registry to {}", savePath.c_str());
}
void AssetRegistrySerializer::Deserialize(std::filesystem::path file, AssetManager& manager)
{

    YAML::Node rootNode = YAML::LoadFile(file);

    if (!rootNode["AssetRegistry"]) {
        DM_CORE_ASSERT("Unable to Load Asset Registry");
    }

    for (auto assetNode : rootNode["AssetRegistry"]) {
        AssetHandle handle = assetNode["handle"].as<u64>();
        std::string path = assetNode["sourcePath"].as<std::string>();
        AssetType type = Asset::assetStringToType(assetNode["type"].as<std::string>());
        AssetMetaData data;
        data.type = type;
        data.sourcePath = path;
        manager.m_Registry[handle] = data;
    }
}
}
