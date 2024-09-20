#include <Assets/AssetManager.hpp>
#include <Rendering/Mesh.hpp>
#include <Rendering/ModelSource.hpp>

namespace Dimensional {

ModelSource::ModelSource(ModelSourceLoadSettings settings)
{
    m_Settings = settings;
    load();
}

void ModelSource::load()
{
    m_Meshes = m_Settings.meshes;
}

}
