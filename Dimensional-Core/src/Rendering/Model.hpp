#ifndef DM_MODEL_H
#define DM_MODEL_H
#include <Assets/Asset.hpp>
#include <Rendering/Mesh.hpp>
#include <core.hpp>

namespace Dimensional {

struct ModelLoadSettings {
    std::vector<Mesh> meshes;
    std::vector<AssetHandle> matHandles;
};

class DMCORE_API Model : public Asset {
public:
    Model(ModelLoadSettings settings);
    ~Model() = default;

    inline std::vector<Mesh>& getMeshes() { return m_Meshes; };

    virtual AssetType getAssetType() const override { return AssetType::MODEL; }

private:
    void load();

    std::vector<Mesh> m_Meshes;
    std::vector<AssetHandle> m_MaterialHandles;

    ModelLoadSettings m_Settings;
};

}
#endif
