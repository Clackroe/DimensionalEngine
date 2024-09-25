#ifndef DM_ENVIRONMENTMAPIMPORTER_HPP
#define DM_ENVIRONMENTMAPIMPORTER_HPP

#include "Assets/AssetMeta.hpp"
#include <Rendering/EnvironmentMap.hpp>
#include <core.hpp>

namespace Dimensional {
class EnvironmentMapImporter {
public:
    static Ref<EnvironmentMap> importEnvironment(AssetMetaData meta);
    static Ref<EnvironmentMap> loadAssetFromPath(std::filesystem::path path);
};
}

#endif
