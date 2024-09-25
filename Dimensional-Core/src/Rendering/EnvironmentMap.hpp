#ifndef DM_ENVIRONMENTMAP_HPP
#define DM_ENVIRONMENTMAP_HPP

#include "Assets/Asset.hpp"
#include "Rendering/IBLMap.hpp"
#include "Rendering/IrMap.hpp"
#include <core.hpp>
namespace Dimensional {

struct EnvironmentMapLoadSettings {
    Ref<IBLMap> irradianceMap;
    Ref<IrMap> radianceMap;
};
class EnvironmentMap : public Asset {
public:
    EnvironmentMap(EnvironmentMapLoadSettings settings);
    ~EnvironmentMap() = default;
    void bind();
    virtual AssetType getAssetType() const { return AssetType::ENVIRONMENTMAP; }

private:
    Ref<IrMap> m_RadianceMap;
    Ref<IBLMap> m_IrradianceMap;
};
}

#endif
