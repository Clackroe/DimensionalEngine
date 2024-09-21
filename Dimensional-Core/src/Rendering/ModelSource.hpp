#ifndef DM_MODELSOURCE_H
#define DM_MODELSOURCE_H
#include <Assets/Asset.hpp>
#include <Rendering/Mesh.hpp>
#include <core.hpp>

namespace Dimensional {

struct ModelSourceLoadSettings {
    std::vector<Mesh> meshes;
    std::vector<std::string> materialNames;
};

class DMCORE_API ModelSource : public Asset {
public:
    ModelSource(ModelSourceLoadSettings settings);
    ~ModelSource() = default;

    inline const std::vector<Mesh>& getMeshes() const { return m_Settings.meshes; };
    inline const std::vector<std::string>& getMaterialNames() const { return m_Settings.materialNames; };

    virtual AssetType getAssetType() const override { return AssetType::MODELSOURCE; }

private:
    void load();

    ModelSourceLoadSettings m_Settings;
};

}
#endif
