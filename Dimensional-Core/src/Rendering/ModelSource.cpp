#include "Log/log.hpp"
#include <Asset/AssetManager.hpp>
#include <Rendering/Mesh.hpp>
#include <Rendering/ModelSource.hpp>

namespace Dimensional {

ModelSource::ModelSource(ModelSourceLoadSettings settings)
{
    m_Settings = settings;
    DM_CORE_WARN("ModeSource Init {}", settings.meshes.size());

    load();
}

void ModelSource::load()
{
}

}
