#ifndef DM_MODEL_H
#define DM_MODEL_H
#include <Assets/Asset.hpp>
#include <Rendering/Mesh.hpp>
#include <core.hpp>

namespace Dimensional {

struct ModelLoadSettings {
    std::vector<Mesh> meshes;
};

class DMCORE_API Model : public Asset {
public:
    Model(ModelLoadSettings settings);
    ~Model() = default;

    inline std::vector<Mesh>& getMeshes() { return m_Meshes; };

private:
    void load();

    std::vector<Mesh> m_Meshes;
    std::vector<AssetHandle> m_MaterialHandles;

    ModelLoadSettings m_Settings;

    friend class Renderer;
};

}
#endif
