#ifndef DM_MODELSOURCE_H
#define DM_MODELSOURCE_H
#include <Assets/Asset.hpp>
#include <Rendering/Mesh.hpp>
#include <core.hpp>

namespace Dimensional {

struct ModelSourceLoadSettings {
    std::vector<Mesh> meshes;
};

class DMCORE_API ModelSource : public Asset {
public:
    ModelSource(ModelSourceLoadSettings settings);
    ~ModelSource() = default;

    inline std::vector<Mesh>& getMeshes() { return m_Meshes; };

    virtual AssetType getAssetType() const override { return AssetType::MODELSOURCE; }

private:
    void load();

    std::vector<Mesh> m_Meshes;

    ModelSourceLoadSettings m_Settings;
};

}
#endif
