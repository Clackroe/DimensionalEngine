#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "yaml-cpp/emitter.h"
#include "yaml-cpp/emittermanip.h"
#include <Scene/EntitySerializer.hpp>
#include <Scene/SceneSerializer.hpp>
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Dimensional {

void SceneSerializer::Serialize(std::filesystem::path savePath, const Ref<Scene> scene)
{
    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "Untitled Dimension";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

    const auto view = scene->m_Registry.view<IDComponent>();
    for (auto& e : view) {
        const Entity entity = { e, scene.get() };
        if (!entity) {
            DM_CORE_WARN("FAILED TO SERIALIZE SCENE");
            return;
        }

        EntitySerialzer::Serialize(out, entity);
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream fout(savePath.c_str());
    fout << out.c_str();
}

}
