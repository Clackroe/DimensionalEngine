
#include <Rendering/EnvironmentMap.hpp>
#include <Rendering/Renderer.hpp>

namespace Dimensional {
EnvironmentMap::EnvironmentMap(EnvironmentMapLoadSettings settings)
{
    m_RadianceMap = settings.radianceMap;
    m_IrradianceMap = settings.irradianceMap;
}

void EnvironmentMap::bind()
{
    m_IrradianceMap->bind(8, 7);
    m_RadianceMap->bind(9);
    // m_IBLMap->bind(8, 7);
    // m_IrMap->bind(9);
}
}
