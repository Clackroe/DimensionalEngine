#ifndef DM_ENTITYSERIALIZER_HPP
#define DM_ENTITYSERIALIZER_HPP
#include "yaml-cpp/emitter.h"
#include <core.hpp>

#include <Scene/Components.hpp>
#include <Scene/Scene.hpp>
#include <yaml-cpp/yaml.h>
namespace Dimensional {
class EntitySerialzer {
public:
    static void Serialize(YAML::Emitter& out, Entity entity);

private:
};
}
#endif
