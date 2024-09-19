// #include <assimp/material.h>
// #include "imgui.h"
#include <Rendering/Mesh.hpp>

#include <Assets/AssetManager.hpp>
#include <Rendering/Model.hpp>

#include <Rendering/Texture.hpp>

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h> // Output data structure

namespace Dimensional {

void Model::Init(std::string path)
{
    loadModel(path);
}

Model::Model(std::string path)
    : Asset(path, AssetType::ModelType)
{
    loadModel(path);
}

void Model::loadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        DM_CORE_ERROR("ASSIMP: {0}", import.GetErrorString());
        return;
    }
    m_Directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(processMesh(mesh, scene));
    }
    // Continue to do the same for all children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        auto& vert = mesh->mVertices[i];
        auto& norm = mesh->mNormals[i];

        Vertex vertex;
        glm::vec3 pos = glm::vec3(vert.x, vert.y, vert.z);
        vertex.Position = pos;

        glm::vec3 normal = glm::vec3(norm.x, norm.y, norm.z);
        vertex.Normal = normal;

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->HasTangentsAndBitangents()) {
            auto& tangent = mesh->mTangents[i];
            auto& biTangent = mesh->mBitangents[i];

            glm::vec3 tan = glm::vec3(tangent.x, tangent.y, tangent.z);
            glm::vec3 bitan = glm::vec3(biTangent.x, biTangent.y, biTangent.z);

            vertex.Tangent = tan;
            vertex.BiTangent = bitan;

        } else {
            vertex.Tangent = glm::vec3(0.0f);
            vertex.BiTangent = glm::vec3(0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    Ref<Material> outMaterial;
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::string matName = name + "_" + material->GetName().C_Str();

        auto mat = AssetManager::getMaterial(matName);
        if (mat) {
            outMaterial = mat;
        }
        outMaterial = AssetManager::loadMaterial(matName);
    }

    return Mesh(vertices, indices, outMaterial);
}
}
