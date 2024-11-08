#ifndef DM_MODEL_H
#define DM_MODEL_H
#include <Asset/Asset.hpp>
#include <Asset/AssetManager.hpp>
#include <Rendering/Mesh.hpp>
#include <Rendering/ModelSource.hpp>
#include <core.hpp>

namespace Dimensional {

struct ModelLoadSettings {
    AssetHandle modelSource = 0;
};

class DMCORE_API Model : public Asset {
public:
    Model(ModelLoadSettings settings);
    ~Model() = default;

    std::vector<Mesh>& getMeshes()
    {
        static std::vector<Mesh> emptyMeshes;

        Ref<ModelSource> source = AssetManager::getInstance().getAsset<ModelSource>(getSource());
        if (source) {
            return source->getMeshes();
        } else {
            return emptyMeshes;
        }
    };

    std::vector<AssetHandle>& getMaterials()
    {
        static std::vector<AssetHandle> emptyAssets;

        Ref<ModelSource> source = AssetManager::getInstance().getAsset<ModelSource>(getSource());
        if (source) {
            return source->getMaterialHandles();
        } else {
            return emptyAssets;
        }
    }

    inline AssetHandle getSource() { return m_Settings.modelSource; }
    inline void setSource(AssetHandle handle) { m_Settings.modelSource = handle; }

    virtual AssetType getAssetType() const override { return AssetType::MODEL; }

private:
    void load();

    ModelLoadSettings m_Settings;
};

}
#endif
