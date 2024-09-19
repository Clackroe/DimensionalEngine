#ifndef DM_MODEL_H
#define DM_MODEL_H
#include <Core/Assets/Asset.hpp>
#include <Rendering/Mesh.hpp>
#include <core.hpp>

class aiMesh;
struct aiScene;
class aiNode;
class aiMaterial;

namespace Dimensional {

class DMCORE_API Model : public Asset {
public:
    Model()
        : Asset("EmptyModel", AssetType::ModelType) {};
    Model(std::string path);
    ~Model() = default;

    void Init(std::string path);

    inline std::vector<Mesh>& getMeshes() { return m_Meshes; };

private:
    std::vector<Mesh> m_Meshes;
    std::string m_Directory;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    friend class Renderer;
};

}
#endif
