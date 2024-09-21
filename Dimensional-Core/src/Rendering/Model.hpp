#ifndef DM_MODEL_H
#define DM_MODEL_H
#include <Assets/Asset.hpp>
#include <Assets/AssetManager.hpp>
#include <Rendering/Mesh.hpp>
#include <Rendering/ModelSource.hpp>
#include <core.hpp>

namespace Dimensional {

struct ModelLoadSettings {
    AssetHandle modelSource = 0;
    std::vector<AssetHandle> matHandles;
};

class DMCORE_API Model : public Asset {
public:
    Model(ModelLoadSettings settings);
    ~Model() = default;

    std::vector<Mesh>& getMeshes()
    {
        static std::vector<Mesh> emptyMeshes;

        Ref<ModelSource> source = AssetManager::getInstance().getAsset<ModelSource>(m_ModelSource);
        if (source) {
            return source->getMeshes();
        } else {
            return emptyMeshes;
        }
    };

    inline AssetHandle getSource() const { return m_ModelSource; }
    inline void setSource(AssetHandle handle) { m_ModelSource = handle; }

    virtual AssetType getAssetType() const override { return AssetType::MODEL; }

private:
    void load();

    AssetHandle m_ModelSource;
    std::vector<AssetHandle> m_MaterialHandles;
    ModelLoadSettings m_Settings;
};

}
#endif
