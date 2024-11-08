#include "Asset/EnvironmentMapImporter.hpp"
#include "Rendering/CubeMap.hpp"
#include "Rendering/EnvironmentMap.hpp"
#include "Rendering/IBLMap.hpp"
#include "Rendering/IrMap.hpp"
#include "Rendering/Shader.hpp"
#include "core.hpp"
namespace Dimensional {

// TODO: Move this out into a location that can be edited.
// May need to create a new asset type called .denvmap or something
// EnvironmentMap:
//    - EnvMapSource: <handle>
//    - Resolution: 1024

static u32 s_Resolution = 1024;
// static u32 s_Resolution = 3072;

Ref<EnvironmentMap> EnvironmentMapImporter::importEnvironment(AssetMetaData meta)
{
    return loadAssetFromPath(meta.sourcePath);
}

Ref<EnvironmentMap> EnvironmentMapImporter::loadAssetFromPath(std::filesystem::path path)
{

    // CreateCubeMap
    Ref<CubeMap> cubeMap = CreateRef<CubeMap>(path.string(), s_Resolution, s_Resolution);

    // Create RadianceMap
    // TODO: Rename to radiance map
    Ref<IrMap> radianceMap = CreateRef<IrMap>(cubeMap);

    // Create IBLMap
    Ref<IBLMap> iblMap = CreateRef<IBLMap>(cubeMap);

    // Create EnvMap
    // Return
    EnvironmentMapLoadSettings settings;
    settings.irradianceMap = iblMap;
    settings.radianceMap = radianceMap;
    Ref<EnvironmentMap> out = CreateRef<EnvironmentMap>(settings);
    return out;
}
}
