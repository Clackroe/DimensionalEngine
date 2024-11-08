#ifndef DM_MODELSOURCE_H
#define DM_MODELSOURCE_H
#include <Asset/Asset.hpp>
#include <Rendering/Mesh.hpp>
#include <core.hpp>

namespace Dimensional {

struct DMCORE_API ModelSourceLoadSettings {
    std::vector<Mesh> meshes;
    std::vector<AssetHandle> meshMaterialHandles;
};

class DMCORE_API ModelSource : public Asset {
public:
    ModelSource(ModelSourceLoadSettings settings);
    ~ModelSource() = default;

    std::vector<Mesh>& getMeshes() { return m_Settings.meshes; };
    std::vector<AssetHandle>& getMaterialHandles() { return m_Settings.meshMaterialHandles; };

    virtual AssetType getAssetType() const override { return AssetType::MODELSOURCE; }

private:
    void load();

    ModelSourceLoadSettings m_Settings;
};

}
#endif
