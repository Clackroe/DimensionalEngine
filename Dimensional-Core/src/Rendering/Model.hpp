#ifndef DM_MODEL_H
#define DM_MODEL_H
#include <Core/Assets/Asset.hpp>
#include <Rendering/Mesh.hpp>
#include <core.hpp>

class aiMesh;
class aiScene;
class aiNode;
class aiMaterial;

namespace Dimensional {

class Model : public Asset {
public:
    Model()
        : Asset("EmptyModel") {};
    Model(std::string path);
    ~Model() = default;

    void Init(std::string path);

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
