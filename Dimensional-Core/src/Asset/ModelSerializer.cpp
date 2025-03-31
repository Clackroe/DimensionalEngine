// #include "Rendering/Model.hpp"
// #include "yaml-cpp/emitter.h"
// #include "yaml-cpp/emittermanip.h"
// #include <Asset/ModelSerializer.hpp>
// #include <yaml-cpp/yaml.h>
//
// namespace Dimensional {
// void ModelSerializer::Serialize(std::filesystem::path savePath, Ref<Model> model)
// {
//     YAML::Emitter out;
//     out << YAML::BeginMap;
//     out << YAML::Key << "Model" << YAML::Value << YAML::BeginMap;
//
//     out << YAML::Key << "modelSource" << YAML::Value << (u64)model->getSource();
//
//     out << YAML::EndMap;
//     out << YAML::EndMap;
//
//     std::ofstream fout(savePath.c_str());
//     fout << out.c_str();
// }
//
// Ref<Model> ModelSerializer::Deserialize(std::filesystem::path file)
// {
//     ModelLoadSettings set;
//
//     YAML::Node rootNode = YAML::LoadFile(file.string());
//
//     auto settings = rootNode["Model"];
//
//     if (!settings) {
//         DM_CORE_ASSERT("Unable to Load Model {0}", file.string());
//     }
//
//     AssetHandle modelSource = settings["modelSource"].as<u64>();
//     set.modelSource = modelSource;
//     return CreateRef<Model>(set);
// }
// }
